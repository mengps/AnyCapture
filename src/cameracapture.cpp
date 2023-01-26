#include "cameracapture.h"
#include "camerasurface.h"

using namespace any_capture;

class CameraCapturePrivate
{
public:
    QCamera *m_captureCamera = nullptr;
    CameraSurface *m_cameraSurface = nullptr;
};

CameraCapture::CameraCapture(const QCameraInfo &info, const QCameraViewfinderSettings &settings, QObject *parent)
    : AbstractCaptureThread(parent)
    , d_ptr(new CameraCapturePrivate)
{
    Q_D(CameraCapture);

    d->m_cameraSurface = new CameraSurface(this);
    d->m_captureCamera = new QCamera(info, this);
    d->m_captureCamera->setViewfinderSettings(settings);
    d->m_captureCamera->setViewfinder(d->m_cameraSurface);
}

CameraCapture::~CameraCapture()
{

}

void CameraCapture::setCaptureParams(OutputVideoFormat format, capture_callback callback, void *master)
{
    Q_D(CameraCapture);

    d->m_cameraSurface->setSurfaceFormat(format, callback, master);
}

QVideoFrame CameraCapture::grabFrame() const
{
    Q_D(const CameraCapture);

    return d->m_cameraSurface->grabFrame();
}

QList<CameraDevice> CameraCapture::availableCameras()
{
    QList<CameraDevice> cameraDevices;
    const auto &cameras = QCameraInfo::availableCameras();
    for (const auto &info: cameras) {
        QCamera camera(info);
        camera.start(); /*! 需start才能取到settings */
        cameraDevices.append({ info, camera.supportedViewfinderSettings() });
    }

    return cameraDevices;
}

void CameraCapture::startCapture()
{
    Q_D(CameraCapture);

    if (d->m_captureCamera->isAvailable()) {
        d->m_captureCamera->start();
    }
}

void CameraCapture::stopCapture()
{
    Q_D(CameraCapture);

    d->m_captureCamera->stop();
}
