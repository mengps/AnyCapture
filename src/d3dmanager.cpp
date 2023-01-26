#include "d3dmanager.h"

#ifdef Q_OS_WIN

#include <QDebug>

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9tex.h>

class D3dManagerPrivate
{
public:
    QString m_lastError;
    D3DDISPLAYMODE m_d3dDisplayMode;
    LPDIRECT3DSURFACE9 m_d3dSurface;
    LPDIRECT3D9 m_d3d9 = nullptr;
    LPDIRECT3DDEVICE9 m_d3d9Device = nullptr;
};

D3dManager::D3dManager()
    : d_ptr(new D3dManagerPrivate)
{

}

D3dManager::~D3dManager()
{
    Q_D(D3dManager);

    if (d->m_d3d9)
        d->m_d3d9->Release();

    if (d->m_d3d9Device)
        d->m_d3d9Device->Release();

    if (d->m_d3dSurface)
        d->m_d3dSurface->Release();
}

bool D3dManager::initialize(QScreen *screen)
{
    Q_D(D3dManager);

    d->m_d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d->m_d3d9) {
        d->m_lastError = "Direct3DCreate9 failed!";
        return false;
    }

    int adapterId = -1;
    auto adapterCount = d->m_d3d9->GetAdapterCount();
    for (uint i = 0; i < adapterCount; i++) {
        D3DADAPTER_IDENTIFIER9 id;
        d->m_d3d9->GetAdapterIdentifier(i, 0, &id);
        if (screen->name() == QString::fromStdString(id.DeviceName)) {
            adapterId = i;
            break;
        }
    }

    if (adapterId == -1) {
        d->m_lastError = "Failed to found screen!";
        return false;
    }

    d->m_d3d9->GetAdapterDisplayMode(adapterId, &d->m_d3dDisplayMode);

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
    d3dpp.Windowed = true;
    d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    d3dpp.BackBufferFormat = d->m_d3dDisplayMode.Format;
    d3dpp.BackBufferHeight = d->m_d3dDisplayMode.Height;
    d3dpp.BackBufferWidth = d->m_d3dDisplayMode.Width;
    d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = GetDesktopWindow();
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
    d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

    d->m_d3d9->CreateDevice(adapterId, D3DDEVTYPE_HAL, GetDesktopWindow()
                            , D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d->m_d3d9Device);

    if (!d->m_d3d9Device) {
        d->m_lastError = "CreateDevice failed!";
        return false;
    } else {
        d->m_d3d9Device->CreateOffscreenPlainSurface(d->m_d3dDisplayMode.Width, d->m_d3dDisplayMode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &d->m_d3dSurface, nullptr);
        return true;
    }
}

QString D3dManager::lastError() const
{
    Q_D(const D3dManager);

    return d->m_lastError;
}

QImage D3dManager::grabScreen()
{
    Q_D(D3dManager);

    QImage grabImage;

    if (d->m_d3d9Device) {
        /*! 桌面只能使用前缓冲 */
        d->m_d3d9Device->GetFrontBufferData(0, d->m_d3dSurface);

        D3DLOCKED_RECT lockedRect;
        d->m_d3dSurface->LockRect(&lockedRect, nullptr, D3DLOCK_NO_DIRTY_UPDATE | D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY);
        grabImage = QImage((uchar *)lockedRect.pBits, d->m_d3dDisplayMode.Width, d->m_d3dDisplayMode.Height, QImage::Format_ARGB32);
        d->m_d3dSurface->UnlockRect();
    }

    return grabImage;
}

#endif
