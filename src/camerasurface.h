#ifndef CAMERASURFACE_H
#define CAMERASURFACE_H

#include "captureglobal.h"

#include <QAbstractVideoSurface>
#include <QVideoFrame>

QT_FORWARD_DECLARE_CLASS(CameraSurfacePrivate);

class CameraSurface : public QAbstractVideoSurface
{
public:
    explicit CameraSurface(QObject *parent = nullptr);
    ~CameraSurface();

    /**
     * @brief 设置表面格式
     * @param format 输出视频图像格式 { @link any_capture::OutputVideoFormat }
     * @param callback 采集回调
     * @param master 采集回调指针
     */
    void setSurfaceFormat(any_capture::OutputVideoFormat format, any_capture::capture_callback callback, void *master);

    QVideoFrame grabFrame() const;

    virtual bool start(const QVideoSurfaceFormat &format) override;

    virtual QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const override;

    virtual bool present(const QVideoFrame &frame) override;

private:
    QScopedPointer<CameraSurfacePrivate> d_ptr;
    Q_DECLARE_PRIVATE(CameraSurface);
};

#endif // CAMERASURFACE_H
