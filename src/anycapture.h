#ifndef ANYCAPTURE_H
#define ANYCAPTURE_H

#include "libanycapture_global.h"
#include "captureglobal.h"

#include <QScreen>

QT_FORWARD_DECLARE_CLASS(AnyCapturePrivate);

class LIBANYCAPTURE_EXPORT AnyCapture : public QObject
{
    Q_OBJECT

public:
    AnyCapture(QObject *parent = nullptr);
    ~AnyCapture();

    /**
     * @brief 获取有效的相机列表
     * @return QList<CameraDevice>
     */
    static QList<any_capture::CameraDevice> availableCameras();

    /**
     * @brief 开始采集相机
     * @param info 要采集的相机信息{使用 @link availableCameras() 获取}
     * @param settings 相机设置
     * @param format 输出图像格式
     * @param callback 采集回调{为空则可使用 @link grabCameraFrame() 抓取}
     * @param master 采集回调指针
     * @return bool{true:成功; false:失败}
     */
    bool startCaptureCamera(const QCameraInfo &info, const QCameraViewfinderSettings &settings, any_capture::OutputVideoFormat format
                            , any_capture::capture_callback callback, void *master);

    /**
     * @brief 抓取一帧相机帧
     * @warning 需要先使用{@link startCaptureCamera()}
     * @return QVideoFrame
     */
    QVideoFrame grabCameraFrame() const;

    /**
     * @brief 停止采集相机
     */
    void stopCaptureCamera();

    /**
     * @brief 获取有效的屏幕列表
     * @return QList<QScreen *>
     */
    static QList<QScreen *> availableScreens();

    /**
     * @brief 开始采集屏幕
     * @param captureScreen 要采集的屏幕{使用 @link availableScreens() 获取}
     * @param captureFrameRate 捕获帧率
     * @param captureCursor 是否捕获光标
     * @param format 输出图像格式
     * @param callback 采集回调{为空则不会采集,但可使用 @link grabScreen() 抓取}
     * @param master 采集回调指针
     * @return bool{true:成功; false:失败}
     */
    bool startCaptureScreen(QScreen *captureScreen, int captureFrameRate, bool captureCursor, any_capture::OutputVideoFormat format
                            , any_capture::capture_callback callback, void *master);

    /**
     * @brief 抓取一帧屏幕
     * @warning 需要先使用{@link startCaptureScreen()}
     * @return QImage
     */
    QImage grabScreen() const;

    /**
     * @brief 停止采集屏幕
     */
    void stopCaptureScreen();

    /**
     * @brief 获取有效的音频输入
     * @return QList<any_capture::AudioDevice>
     */
    static QList<any_capture::AudioDevice> availableAudioInputs();

    /**
     * @brief 开始采集音频输入
     * @param input 要采集的音频输入
     * @param callback 采集回调
     * @param master 采集回调指针
     * @return bool{true:成功; false:失败}
     */
    bool startCaptureAudioInput(const any_capture::AudioDevice &input, any_capture::capture_callback callback, void *master);

    /**
     * @brief 停止采集音频输入
     */
    void stopCaptureAudioInput();

    /**
     * @brief 获取有效的音频输出
     * @return QList<any_capture::AudioDevice>
     */
    static QList<any_capture::AudioDevice> availableAudioOutputs();

    /**
     * @brief 开始采集音频输出
     * @param output 要采集的音频输出
     * @param callback 采集回调
     * @param master 采集回调指针
     * @return bool{true:成功; false:失败}
     */
    bool startCaptureAudioOutput(const any_capture::AudioDevice &output, any_capture::capture_callback callback, void *master);

    /**
     * @brief 停止采集音频输出
     */
    void stopCaptureAudioOutput();

private:
    QScopedPointer<AnyCapturePrivate> d_ptr;
    Q_DECLARE_PRIVATE(AnyCapture);
};

#endif // ANYCAPTURE_H
