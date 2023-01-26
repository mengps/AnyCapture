#include "audiocapture.h"

#include <QDebug>

#ifdef Q_OS_WIN
#include <mmdeviceapi.h>
#include <propvarutil.h>
#include <Audioclient.h>
#include <Audiopolicy.h>
#include <Functiondiscoverykeys_devpkey.h>
#endif

using namespace any_capture;

#ifdef Q_OS_WIN

static QString getDeviceId(IMMDevice *device)
{
    LPWSTR buffer = nullptr;
    if (SUCCEEDED(device->GetId(&buffer))) {
        return QString::fromWCharArray(buffer);
    }
    return QString();
}

static QString getDeviceState(IMMDevice *device)
{
    DWORD state = 0;
    if (SUCCEEDED(device->GetState(&state))) {
        switch (state) {
        case DEVICE_STATE_ACTIVE: return "Active";
        case DEVICE_STATE_DISABLED: return "Disabled";
        case DEVICE_STATE_NOTPRESENT: return "NotPresent";
        case DEVICE_STATE_UNPLUGGED: return "UnPlugged";
        }
    }
    return "Unknown";
}

static QString getPropString(IPropertyStore *prop, const PROPERTYKEY &key)
{
    PROPVARIANT var;
    PropVariantInit(&var);
    QString propString;
    if (SUCCEEDED(prop->GetValue(key, &var))) {
        WCHAR title[128] = { 0 };
        PropVariantToString(var, title, ARRAYSIZE(title));
        propString = QString::fromWCharArray(title);
    }
    PropVariantClear(&var);

    return propString;
}

static void adjustFormat(WAVEFORMATEX *waveFormat)
{
    if (waveFormat->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) {
        waveFormat->wFormatTag = WAVE_FORMAT_PCM;
        waveFormat->wBitsPerSample = 32;
        waveFormat->nBlockAlign = waveFormat->nChannels * waveFormat->wBitsPerSample / 8;
        waveFormat->nAvgBytesPerSec = waveFormat->nBlockAlign * waveFormat->nSamplesPerSec;
    } else if (waveFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        PWAVEFORMATEXTENSIBLE exFormat = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(waveFormat);
        if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, exFormat->SubFormat)) {
            exFormat->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
            exFormat->Samples.wValidBitsPerSample = 32;
            waveFormat->wBitsPerSample = 32;
            waveFormat->nBlockAlign = waveFormat->nChannels * waveFormat->wBitsPerSample / 8;
            waveFormat->nAvgBytesPerSec = waveFormat->nBlockAlign * waveFormat->nSamplesPerSec;
        }
    }
}

#endif

class AudioCapturePrivate
{
public:
    QString m_lastError;
    AudioDevice m_captureDevice;
    capture_callback m_callback = nullptr;
    void *m_master = nullptr;
    uint32_t m_frameSize = 0;
};

AudioCapture::AudioCapture(const AudioDevice &device, QObject *parent)
    : AbstractCaptureThread(parent)
    , d_ptr(new AudioCapturePrivate)
{
    Q_D(AudioCapture);

    d->m_captureDevice = device;
}

AudioCapture::~AudioCapture()
{
    stopCapture();
#ifdef Q_OS_WIN
    CoUninitialize();
#endif
}

void AudioCapture::setCaptureParams(capture_callback callback, void *master)
{
    Q_D(AudioCapture);

    d->m_callback = callback;
    d->m_master = master;
}

QString AudioCapture::lastError() const
{
    Q_D(const AudioCapture);

    return d->m_lastError;
}

uint32_t AudioCapture::frameSize() const
{
    Q_D(const AudioCapture);

    return d->m_frameSize;
}

QList<AudioDevice> AudioCapture::availableAudios(AudioType type)
{
    QList<AudioDevice> devices;

#ifdef Q_OS_WIN
    CoInitializeEx(nullptr, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY);

    HRESULT hr;
    IMMDeviceEnumerator *deviceEnumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
    if (FAILED(hr)) {
        qDebug() << "Failed to CoCreateInstance ErrorCode = " + QString::number(uint(hr), 16);
        return devices;
    }

    IMMDeviceCollection *deviceCollection = nullptr;
    if (type == AudioType::Audio_Input)
        hr = deviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &deviceCollection);
    else
        hr = deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &deviceCollection);

    if (FAILED(hr)) {
        qDebug() << "Failed to EnumAudioEndpoints ErrorCode = " + QString::number(uint(hr), 16);
        deviceEnumerator->Release();
        return devices;
    }

    UINT deviceCount = 0;
    hr = deviceCollection->GetCount(&deviceCount);
    if (FAILED(hr)) {
        qDebug() << "Failed to GetCount ErrorCode = " + QString::number(uint(hr), 16);
        return devices;
    }

    for (UINT i = 0 ; i < deviceCount; i++) {
        IMMDevice *device = nullptr;
        hr = deviceCollection->Item(i, &device);
        if (SUCCEEDED(hr)) {
            auto const id = getDeviceId(device);
            auto const state = getDeviceState(device);
            IPropertyStore *prop = nullptr;
            if (SUCCEEDED(device->OpenPropertyStore(STGM_READ, &prop))) {
                auto const name = getPropString(prop, PKEY_Device_FriendlyName);
                auto const desc = getPropString(prop, PKEY_Device_DeviceDesc);
                auto const audioif = getPropString(prop, PKEY_DeviceInterface_FriendlyName);
                IAudioClient *audioClient = nullptr;
                HRESULT hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void **>(&audioClient));
                if (SUCCEEDED(hr)) {
                    WAVEFORMATEX *mixFormat;
                    hr = audioClient->GetMixFormat(&mixFormat);
                    if (SUCCEEDED(hr)) {
                        adjustFormat(mixFormat);
                        int sampleSize = mixFormat->wBitsPerSample;
                        if (mixFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
                            WAVEFORMATEXTENSIBLE *exFormat = reinterpret_cast<WAVEFORMATEXTENSIBLE *>(mixFormat);
                            sampleSize = exFormat->Samples.wValidBitsPerSample;
                        }
                        AudioDevice audioDevice;
                        audioDevice.type = type;
                        audioDevice.channels = mixFormat->nChannels;
                        audioDevice.sampleSize = sampleSize;
                        audioDevice.sampleRate = mixFormat->nSamplesPerSec;
                        audioDevice.deviceHandle = device;
                        audioDevice.deviceName = name;
                        audioDevice.deviceDescription = desc;
                        devices.append(audioDevice);
                    }
                    audioClient->Release();
                }
            }
        }
    }

    deviceEnumerator->Release();
    deviceCollection->Release();
#endif

    return devices;
}

void AudioCapture::run()
{
    Q_D(AudioCapture);

    if (d->m_captureDevice.deviceHandle) {
#ifdef Q_OS_WIN
        IMMDevice *device = static_cast<IMMDevice *>(d->m_captureDevice.deviceHandle);
        IAudioClient *audioClient = nullptr;
        HRESULT hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void **>(&audioClient));
        if (SUCCEEDED(hr)) {
            WAVEFORMATEX *mixFormat;
            hr = audioClient->GetMixFormat(&mixFormat);
            if (FAILED(hr)) {
                d->m_lastError = "Failed to GetMixFormat ErrorCode = " + QString::number(uint(hr), 16);
            }

            adjustFormat(mixFormat);

            hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED
                                         , d->m_captureDevice.type == AudioType::Audio_Output ? AUDCLNT_STREAMFLAGS_LOOPBACK : 0
                                         , 20 * 10000, 0, mixFormat, nullptr);
            if (FAILED(hr)) {
                d->m_lastError = "Failed to Initialize audioClient ErrorCode = " + QString::number(uint(hr), 16);
                return;
            }

            hr = audioClient->GetBufferSize(&d->m_frameSize);
            if (FAILED(hr)) {
                d->m_lastError = "Failed to GetBufferSize ErrorCode = " + QString::number(uint(hr), 16);
                return;
            }

            IAudioCaptureClient *captureClient;
            hr = audioClient->GetService(IID_PPV_ARGS(&captureClient));
            if (FAILED(hr)) {
                d->m_lastError = "Failed to GetService ErrorCode = " + QString::number(uint(hr), 16);
                return;
            }

            /*! 开始采集 */
            hr = audioClient->Start();
            if (FAILED(hr)) {
                d->m_lastError = "Failed to Start audioClient ErrorCode = " + QString::number(uint(hr), 16);
                return;
            }

            BYTE *data;
            DWORD flags;
            UINT32 framesAvailable;
            UINT32 packetLength = 0;
            size_t frameSize = (mixFormat->wBitsPerSample / 8) * mixFormat->nChannels;
            while (m_captrueThreadRunning) {
                hr = captureClient->GetNextPacketSize(&packetLength);
                if (FAILED(hr))
                    goto Capture_End;

                while (packetLength != 0) {
                    hr = captureClient->GetBuffer(&data, &framesAvailable, &flags, nullptr, nullptr);
                    if (FAILED(hr))
                        goto Capture_End;

                    if (framesAvailable!= 0) {
                        if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {

                        } else {
                            if (d->m_callback) {
                                d->m_callback(data, framesAvailable * frameSize, d->m_master);
                            }
                        }
                    }

                    hr = captureClient->ReleaseBuffer(framesAvailable);
                    if (FAILED(hr)) {
                        qDebug() << __func__ << "Failed to ReleaseBuffer ErrorCode = " + QString::number(uint(hr), 16);
                        goto Capture_End;
                    }

                    hr = captureClient->GetNextPacketSize(&packetLength);
                    if (FAILED(hr))
                        goto Capture_End;
                }

                timeBeginPeriod(1);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                timeEndPeriod(1);
            }
        }
Capture_End:
        device->Release();
#endif
    }
}
