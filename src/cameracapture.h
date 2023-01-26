#ifndef CAMERACAPTURE_H
#define CAMERACAPTURE_H

#include "abstractcapturethread.h"
#include "captureglobal.h"

#include <QCamera>

QT_FORWARD_DECLARE_CLASS(CameraCapturePrivate);

class CameraCapture : public AbstractCaptureThread
{
public:
    CameraCapture(const QCameraInfo &info, const QCameraViewfinderSettings &settings, QObject *parent = nullptr);
    ~CameraCapture();

    void setCaptureParams(any_capture::OutputVideoFormat format, any_capture::capture_callback callback, void *master);

    QVideoFrame grabFrame() const;

    static QList<any_capture::CameraDevice> availableCameras();

    virtual void startCapture() override;

    virtual void stopCapture() override;

private:
    QScopedPointer<CameraCapturePrivate> d_ptr;
    Q_DECLARE_PRIVATE(CameraCapture);
};

#endif // CAMERACAPTURE_H
