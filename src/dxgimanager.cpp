#include "dxgimanager.h"

#ifdef Q_OS_WIN

#include <QDebug>

#include <windows.h>
#include <dxgi1_6.h>
#include <d3d11.h>

class Texture
{
public:
    virtual ~Texture() { m_texture->Release(); }
    virtual QImage copyToImage(IDXGIResource *res) = 0;

protected:
    ID3D11Texture2D *m_texture = nullptr;
};

class DxgiTextureStaging : public Texture
{
public:
    DxgiTextureStaging(ID3D11Device *device, ID3D11DeviceContext *context)
        : m_device(device), m_context(context)
    {

    }
    ~DxgiTextureStaging()
    {
        m_device->Release();
        m_context->Release();
    }

    QImage copyToImage(IDXGIResource *res);

private:
    ID3D11Device *m_device = nullptr;
    ID3D11DeviceContext * m_context = nullptr;
};

QImage DxgiTextureStaging::copyToImage(IDXGIResource *res)
{
    D3D11_TEXTURE2D_DESC desc;
    ID3D11Texture2D *textrueRes = nullptr;
    HRESULT hr = res->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&textrueRes));
    if (FAILED(hr)) {
        qDebug() << "Failed to ID3D11Texture2D result =" << Qt::hex << uint(hr);
        return QImage();
    }
    textrueRes->GetDesc(&desc);

    D3D11_TEXTURE2D_DESC texDesc;
    ZeroMemory(&texDesc, sizeof(texDesc));
    texDesc.Width = desc.Width;
    texDesc.Height = desc.Height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_STAGING;
    texDesc.Format = desc.Format;
    texDesc.BindFlags = 0;
    texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    texDesc.MiscFlags = 0;
    m_device->CreateTexture2D(&texDesc, nullptr, &m_texture);
    m_context->CopyResource(m_texture, textrueRes);

    IDXGISurface1 *surface = nullptr;
    hr = m_texture->QueryInterface(__uuidof(IDXGISurface1), reinterpret_cast<void **>(&surface));
    if (FAILED(hr)) {
        qDebug() << "Failed to QueryInterface IDXGISurface1 ErrorCode =" << Qt::hex << uint(hr);
        return QImage();
    }

    DXGI_MAPPED_RECT map;
    surface->Map(&map, DXGI_MAP_READ);
    QImage grabImage(static_cast<uchar *>(map.pBits)
                     , int(desc.Width), int(desc.Height)
                     , QImage::Format_ARGB32);
    surface->Unmap();
    surface->Release();
    m_texture->Release();

    return grabImage;
}

class DxgiManagerPrivate
{
public:
    QString m_lastError;
    Texture *m_texture = nullptr;
    IDXGIOutputDuplication *m_duplication = nullptr;
};

DxgiManager::DxgiManager()
    : d_ptr(new DxgiManagerPrivate)
{

}

DxgiManager::~DxgiManager()
{

}

bool DxgiManager::initialize(QScreen *screen)
{
    Q_D(DxgiManager);

    if (!screen) {
        d->m_lastError = "Screen is invaild!";
        return false;
    }

    ID3D11Device *d3dDevice = nullptr;
    ID3D11DeviceContext *d3dContext = nullptr;
    D3D_FEATURE_LEVEL feat = D3D_FEATURE_LEVEL_11_0;
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &d3dDevice, &feat, &d3dContext);
    if (FAILED(hr)) {
        d->m_lastError = "Failed to D3D11CreateDevice ErrorCode = " + QString::number(uint(hr), 16);
        return false;
    }

    IDXGIDevice *dxgiDevice = nullptr;
    hr = d3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
    if(FAILED(hr)) {
        d->m_lastError = "Failed to QueryInterface IDXGIOutput6 ErrorCode = " + QString::number(uint(hr), 16);
        return false;
    }

    IDXGIAdapter *dxgiAdapter = nullptr;
    hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter));
    dxgiDevice->Release();
    if (FAILED(hr)) {
        d->m_lastError = "Failed to Get IDXGIAdapter ErrorCode = " + QString::number(uint(hr), 16);
        return false;
    }

    IDXGIOutput *dxgiOutput = nullptr;
    QVector<IDXGIOutput *> outputs;
    for(uint i = 0; dxgiAdapter->EnumOutputs(i, &dxgiOutput) != DXGI_ERROR_NOT_FOUND; ++i) {
        outputs.push_back(dxgiOutput);
    }
    dxgiAdapter->Release();

    dxgiOutput = nullptr;
    for (const auto &output: outputs) {
        DXGI_OUTPUT_DESC desc;
        output->GetDesc(&desc);
        if (screen->name() == QString::fromWCharArray(desc.DeviceName)) {
            dxgiOutput = output;
            break;
        }
    }

    if (!dxgiOutput) {
        d->m_lastError = "Failed to found screen!";
        return false;
    }

    IDXGIOutput6 *dxgiOutput6 = nullptr;
    hr = dxgiOutput->QueryInterface(__uuidof(IDXGIOutput6), reinterpret_cast<void**>(&dxgiOutput6));
    dxgiOutput->Release();
    if (FAILED(hr)) {
        d->m_lastError = "Failed to QueryInterface IDXGIOutput6 ErrorCode = " + QString::number(uint(hr), 16);
        return false;
    }

    hr = dxgiOutput6->DuplicateOutput(d3dDevice, &d->m_duplication);
    dxgiOutput6->Release();
    if (FAILED(hr)) {
        d->m_lastError = "Failed to DuplicateOutput ErrorCode = " + QString::number(uint(hr), 16);
        return false;
    }

    DXGI_OUTDUPL_DESC desc;
    d->m_duplication->GetDesc(&desc);
    d->m_texture = new DxgiTextureStaging(d3dDevice, d3dContext);
    if (desc.DesktopImageInSystemMemory) {
        qDebug() << "Desc: CPU shared with GPU";
    } else {
        qDebug() << "Desc: CPU not shared with GPU";
    }

    return true;
}

QString DxgiManager::lastError() const
{
    Q_D(const DxgiManager);

    return d->m_lastError;
}

QImage DxgiManager::grabScreen()
{
    Q_D(DxgiManager);

    IDXGIResource *desktopRes;
    DXGI_OUTDUPL_FRAME_INFO frameInfo;
    while (true) {
        d->m_duplication->ReleaseFrame();
        HRESULT hr = d->m_duplication->AcquireNextFrame(30, &frameInfo, &desktopRes);
        if (FAILED(hr)) {
            d->m_lastError = "Failed to AcquireNextFrame ErrorCode = " + QString::number(uint(hr), 16);
            return QImage();
        }

        if (frameInfo.LastPresentTime.QuadPart) break;
    }

    return d->m_texture->copyToImage(desktopRes);
}

#endif
