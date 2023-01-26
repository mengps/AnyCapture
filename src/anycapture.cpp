#include "anycapture.h"
#include "audiocapture.h"
#include "cameracapture.h"
#include "screencapture.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

using namespace any_capture;

class AnyCapturePrivate
{
public:
    QSharedPointer<AudioCapture> m_audioInputCapture, m_audioOutputCapture;
    QSharedPointer<ScreenCapture> m_screenCapture;
    QSharedPointer<CameraCapture> m_cameraCapture;
};

AnyCapture::AnyCapture(QObject *parent)
    : QObject(parent)
    , d_ptr(new AnyCapturePrivate)
{

}

AnyCapture::~AnyCapture()
{
    stopCaptureCamera();
    stopCaptureScreen();
    stopCaptureAudioInput();
    stopCaptureAudioOutput();
}

QList<CameraDevice> AnyCapture::availableCameras()
{
    return CameraCapture::availableCameras();
}

bool AnyCapture::startCaptureCamera(const QCameraInfo &info, const QCameraViewfinderSettings &settings, OutputVideoFormat format, capture_callback callback, void *master)
{
    Q_D(AnyCapture);

    stopCaptureCamera();

    if (info.isNull() || settings.isNull()) return false;

    d->m_cameraCapture.reset(new CameraCapture(info, settings));
    d->m_cameraCapture->setCaptureParams(format, callback, master);
    if (callback)
        d->m_cameraCapture->startCapture();

    return true;
}

QVideoFrame AnyCapture::grabCameraFrame() const
{
    Q_D(const AnyCapture);

    if (!d->m_cameraCapture.isNull())
        return d->m_cameraCapture->grabFrame();

    return QVideoFrame();
}

void AnyCapture::stopCaptureCamera()
{
    Q_D(AnyCapture);

    if (!d->m_cameraCapture.isNull())
        d->m_cameraCapture->stopCapture();
}

QList<QScreen *> AnyCapture::availableScreens()
{
    return ScreenCapture::availableScreens();
}

bool AnyCapture::startCaptureScreen(QScreen *captureScreen, int captureFrameRate, bool captureCursor, OutputVideoFormat format, capture_callback callback, void *master)
{
    Q_D(AnyCapture);

    stopCaptureScreen();

    if (!captureScreen) return false;

    d->m_screenCapture.reset(new ScreenCapture(ScreenCapture::CaptureMode::Mode_Auto, captureScreen));
    d->m_screenCapture->setCaptureParams(format, captureFrameRate, captureCursor, callback, master);
    if (callback)
        d->m_screenCapture->startCapture();

    return true;
}

QImage AnyCapture::grabScreen() const
{
    Q_D(const AnyCapture);

    if (!d->m_screenCapture.isNull())
        return d->m_screenCapture->grabScreen();
    else
        return QImage();
}

void AnyCapture::stopCaptureScreen()
{
    Q_D(AnyCapture);

    if (!d->m_screenCapture.isNull())
        d->m_screenCapture->stopCapture();
}

QList<AudioDevice> AnyCapture::availableAudioInputs()
{
    return AudioCapture::availableAudios(AudioType::Audio_Input);
}

bool AnyCapture::startCaptureAudioInput(const AudioDevice &input, capture_callback callback, void *master)
{
    Q_D(AnyCapture);

    stopCaptureAudioInput();

    if (input.deviceHandle == nullptr) {
        return false;
    }

    d->m_audioInputCapture.reset(new AudioCapture(input));
    d->m_audioInputCapture->setCaptureParams(callback, master);
    if (callback)
        d->m_audioInputCapture->startCapture();

    return true;
}

void AnyCapture::stopCaptureAudioInput()
{
    Q_D(AnyCapture);

    if (!d->m_audioInputCapture.isNull())
        d->m_audioInputCapture->stopCapture();
}

QList<AudioDevice> AnyCapture::availableAudioOutputs()
{
    return AudioCapture::availableAudios(AudioType::Audio_Output);
}

bool AnyCapture::startCaptureAudioOutput(const AudioDevice &output, capture_callback callback, void *master)
{
    Q_D(AnyCapture);

    stopCaptureAudioOutput();

    if (output.deviceHandle == nullptr) {
        return false;
    }

    d->m_audioOutputCapture.reset(new AudioCapture(output));
    d->m_audioOutputCapture->setCaptureParams(callback, master);
    if (callback)
        d->m_audioOutputCapture->startCapture();

    return true;
}

void AnyCapture::stopCaptureAudioOutput()
{
    Q_D(AnyCapture);

    if (!d->m_audioOutputCapture.isNull())
        d->m_audioOutputCapture->stopCapture();
}
