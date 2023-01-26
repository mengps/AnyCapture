#ifndef CAPTUREGLOBAL_H
#define CAPTUREGLOBAL_H

#include <QCameraInfo>

namespace any_capture {

enum class OutputVideoFormat {
    Format_RGB24,
    Format_YUV420P
};

struct CameraDevice
{
    QCameraInfo cameraInfo; /*! 相机信息 */
    QList<QCameraViewfinderSettings> supportedSettings; /*! 支持的设置 */
};

enum class AudioType
{
    Audio_Input, /*! 音频输入 */
    Audio_Output /*! 音频输出 */
};

struct AudioDevice
{
    AudioType type; /*! 通道数 */
    int channels;   /*! 通道数 */
    int sampleSize; /*! 样本大小 */
    int sampleRate; /*! 采样率 */
    void *deviceHandle = nullptr; /*! 本机设备句柄 windows:IMMDevice* */
    QString deviceName; /*! 设备名称 */
    QString deviceDescription; /*! 设备描述 */
};

/*! 采集回调 */
typedef int (*capture_callback)(const unsigned char *data, size_t dataSize, void *master);

};

#endif // CAPTUREGLOBAL_H
