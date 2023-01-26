#ifndef AUDIOCAPTURE_H
#define AUDIOCAPTURE_H

#include "abstractcapturethread.h"
#include "captureglobal.h"

QT_FORWARD_DECLARE_CLASS(AudioCapturePrivate);

class AudioCapture : public AbstractCaptureThread
{
public:
    AudioCapture(const any_capture::AudioDevice &device, QObject *parent = nullptr);
    ~AudioCapture();

    void setCaptureParams(any_capture::capture_callback callback, void *master);

    QString lastError() const;

    uint32_t frameSize() const;

    static QList<any_capture::AudioDevice> availableAudios(any_capture::AudioType type);

protected:
    virtual void run() override;

private:
    QScopedPointer<AudioCapturePrivate> d_ptr;
    Q_DECLARE_PRIVATE(AudioCapture);
};

#endif // AUDIOCAPTURE_H
