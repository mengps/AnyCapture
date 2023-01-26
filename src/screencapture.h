#ifndef SCREENCAPTURE_H
#define SCREENCAPTURE_H

#include "abstractcapturethread.h"
#include "captureglobal.h"

#include <QScreen>

QT_FORWARD_DECLARE_CLASS(ScreenCapturePrivate);

class ScreenCapture : public AbstractCaptureThread
{
public:
    /**
     * @brief 采集模式 enum
     */
    enum class CaptureMode {
        Mode_Auto,    /*! 自动选择 */
        Mode_DXGI,    /*! 使用DXGI */
        Mode_D3D,     /*! 使用D3D */
        Mode_Defalut  /*! 使用Defalut */
    };

    /**
     * @brief ScreenCapture
     * @param preferenceMode 首选模式
     * @param screen 要采集的屏幕
     */
    ScreenCapture(CaptureMode preferenceMode, QScreen *screen, QObject *parent = nullptr);
    ~ScreenCapture();

    void setCaptureParams(any_capture::OutputVideoFormat format, int captureFrameRate, bool captureCursor
                          , any_capture::capture_callback callback, void *master);
    /**
     * @brief 返回当前的采集模式
     * @return CaptureMode{ @link CaptureMode }
     */
    CaptureMode captureMode() const;

    /**
     * @brief 抓取一帧屏幕
     * @return QImage
     */
    QImage grabScreen();

    /**
     * @brief 返回有效的屏幕列表
     * @return QList<QScreen *>
     */
    static QList<QScreen *> availableScreens();

protected:
    virtual void run() override;

private:
    QScopedPointer<ScreenCapturePrivate> d_ptr;
    Q_DECLARE_PRIVATE(ScreenCapture);
};

#endif // SCREENCAPTURE_H
