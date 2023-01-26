#include "screencapture.h"
#include "d3dmanager.h"
#include "dxgimanager.h"
#include "yuvconvertor.h"

#include <QDebug>
#include <QPainter>
#include <QGuiApplication>
#include <QTime>

#ifdef Q_OS_WIN
#include <Windows.h>
#include <QtWin>
#endif

using namespace any_capture;

static void high_resolution_sleep_for(int milliseconds)
{
#ifdef Q_OS_WIN
    timeBeginPeriod(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    timeEndPeriod(1);
#elif
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
#endif
}

static void drawCursor(QImage &screen, const QPoint &start)
{
    QPixmap cursorPixmap;
    QPoint cursorPosition;
    QSize cursorSize;

#ifdef Q_OS_WIN
    /*! 获取光标大小 */
    int cursorWidth = GetSystemMetrics(SM_CXCURSOR);
    int cursorHeight = GetSystemMetrics(SM_CYCURSOR);
    cursorSize = QSize(cursorWidth, cursorHeight);

    /*! 获取设备上下文 */
    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    /*! 创建位图以用作画布 */
    HBITMAP hbmCanvas = CreateCompatibleBitmap(hdcScreen, cursorWidth, cursorHeight);

    /*! 在设备上下文中选择位图 */
    HGDIOBJ hbmOld = SelectObject(hdcMem, hbmCanvas);

    /*! 获取有关全局光标的信息 */
    CURSORINFO ci;
    ci.cbSize = sizeof(ci);

    /*! 将光标绘制到画布中 */
    if (GetCursorInfo(&ci)) {
        ICONINFO ii = { 0 };
        cursorPosition = QPoint(ci.ptScreenPos.x - ii.xHotspot, ci.ptScreenPos.y - ii.yHotspot);
        GetIconInfo(ci.hCursor, &ii);
        DeleteObject(ii.hbmColor);
        DeleteObject(ii.hbmMask);
        /*! 注意 光标需要混合背景才能得到正确的颜色 */
        BitBlt(hdcMem, 0, 0, cursorWidth, cursorHeight, hdcScreen, cursorPosition.x(), cursorPosition.y(), SRCCOPY);
        DrawIcon(hdcMem, 0, 0, ci.hCursor);
    }

    cursorPixmap = QtWin::fromHBITMAP(hbmCanvas, QtWin::HBitmapNoAlpha);

    /*! 清除 */
    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbmCanvas);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
#endif

    QPainter painter(&screen);
    auto position = cursorPosition - start;
    if (position.x() >= 0 && position.y() >= 0) {
        painter.drawPixmap(QRect(position, cursorSize), cursorPixmap);
    }
}

class ScreenCapturePrivate
{
public:
#ifdef Q_OS_WIN
    DxgiManager m_dxgiManager;
    D3dManager m_d3dManager;
#endif
    bool m_captureCursor = false;
    int m_captureFrameRate = 25;
    OutputVideoFormat m_outputFormat;
    capture_callback m_callback = nullptr;
    void *m_master = nullptr;
    QScreen *m_captureScreen = nullptr;
    std::vector<unsigned char> m_convertCache;
    ScreenCapture::CaptureMode m_captureMode = ScreenCapture::CaptureMode::Mode_Auto;
};

ScreenCapture::ScreenCapture(CaptureMode preferenceMode, QScreen *screen, QObject *parent)
    : AbstractCaptureThread(parent)
    , d_ptr(new ScreenCapturePrivate)
{
    Q_D(ScreenCapture);

    d->m_captureScreen = screen;

#ifdef Q_OS_WIN
    d->m_captureMode = preferenceMode;

    if (d->m_captureMode == CaptureMode::Mode_Auto || d->m_captureMode == CaptureMode::Mode_DXGI) {
        if (d->m_dxgiManager.initialize(d->m_captureScreen)) {
            d->m_captureMode = CaptureMode::Mode_DXGI;
        } else {
            qDebug() << __func__ << d->m_dxgiManager.lastError();
            /*! 切换到D3D */
            d->m_captureMode = CaptureMode::Mode_D3D;
        }
    }

    if (d->m_captureMode == CaptureMode::Mode_D3D) {
        if (!d->m_d3dManager.initialize(d->m_captureScreen)) {
            qDebug() << __func__ << d->m_d3dManager.lastError();
            /*! 切换为默认 */
            d->m_captureMode = CaptureMode::Mode_Defalut;
        }
    }

#else
    /*! 其他平台统一使用默认 */
    d->m_captureMode = CaptureMode::Mode_Defalut;
#endif
}

ScreenCapture::~ScreenCapture()
{
    stopCapture();
}

void ScreenCapture::setCaptureParams(OutputVideoFormat format, int captureFrameRate, bool captureCursor, capture_callback callback, void *master)
{
    Q_D(ScreenCapture);

    if (d->m_captureScreen) {
        d->m_outputFormat = format;
        d->m_captureCursor = captureCursor;
        d->m_captureFrameRate = captureFrameRate;
        d->m_callback = callback;
        d->m_master = master;

        auto size = d->m_captureScreen->geometry().size();
        if (d->m_outputFormat == OutputVideoFormat::Format_RGB24) {
            d->m_convertCache.resize(size.width() * size.height() * 3);
        } else if (d->m_captureScreen && format == OutputVideoFormat::Format_YUV420P) {
            d->m_convertCache.resize(size.width() * size.height() * 1.5);
        }
    }
}

ScreenCapture::CaptureMode ScreenCapture::captureMode() const
{
    Q_D(const ScreenCapture);

    return d->m_captureMode;
}

QImage ScreenCapture::grabScreen()
{
    Q_D(ScreenCapture);

    QImage grabImage;
    if (d->m_captureScreen) {
#ifdef Q_OS_WIN
        switch (d->m_captureMode) {
        case CaptureMode::Mode_DXGI:
            grabImage = d->m_dxgiManager.grabScreen();
            break;
        case CaptureMode::Mode_D3D:
            grabImage = d->m_d3dManager.grabScreen();
            break;
        case CaptureMode::Mode_Defalut:
        default:
            grabImage = d->m_captureScreen->grabWindow(0).toImage();
            break;
        }
#else
        grabImage = d->m_captureScreen->grabWindow(0).toImage();
#endif

        if (d->m_captureCursor && !grabImage.isNull()) {
            drawCursor(grabImage, d->m_captureScreen->geometry().topLeft());
        }
    }

    return grabImage;
}

QList<QScreen *> ScreenCapture::availableScreens()
{
    return QGuiApplication::screens();
}

void ScreenCapture::run()
{
    Q_D(ScreenCapture);

    Q_ASSERT_X(d->m_captureFrameRate > 0, "ScreenCapture", "captureFrameRate <= 0");

    int captureInterval = 1000 / d->m_captureFrameRate;
    QTime updateTime = QTime::currentTime().addMSecs(-captureInterval);
    while (m_captrueThreadRunning) {
        if (updateTime.msecsTo(QTime::currentTime()) >= captureInterval) {
            updateTime = QTime::currentTime();
            if (d->m_callback) {
                const auto &image = grabScreen();
                if (d->m_outputFormat == OutputVideoFormat::Format_RGB24) {
                    YUVConvertor::argb32ToRgb24((uchar *)image.constBits(), image.width(), image.height(), &d->m_convertCache[0]);
                    d->m_callback(&d->m_convertCache[0], d->m_convertCache.size(), d->m_master);
                } else if (d->m_outputFormat == OutputVideoFormat::Format_YUV420P) {
                    YUVConvertor::argb32ToI420((uchar *)image.constBits(), image.width(), image.height(), &d->m_convertCache[0]);
                    d->m_callback(&d->m_convertCache[0], d->m_convertCache.size(), d->m_master);
                }
            }
        }
        high_resolution_sleep_for(1);
    }
}

