# AnyCapture

  `AnyCapture` 是一个基于 Qt 的回调式采集库。

  它简化了各类采集实现(包装)，提供相当方便的接口，旨在能够轻松集成以实现采集相关工作。

---

### 采集目标

 - 轻松采集屏幕, `ScreenCapture` 提供多种采集模式(GDI/DX9/DXGI)，`AnyCapture` 默认将自动选择。

 - 轻松采集相机, `CameraCapture` 使用 `Qt Multimedia` 完成采集工作。

 - 轻松采集音频输入&输出, `AudioCapture` 使用 WASAPI(Windows)。

 最多支持采集1个屏幕&1个相机&1个音频输入&1个音频输出, 然而可简单实例多个(无需担心资源占用)实现多路采集。

---

### 如何构建

 1. 构建 `libyuvconvertor`, 为 `libyuv` 浅包装, 构建文件 `/libyuvconvertor/libyuvconvertor.pro`。

 2. 构建 `anycapture`, 构建文件 `anycapture.pro`。

---

### 如何使用

  1. 使用 availableCameras() / availableScreens() / availableAudioInputs() / availableAudioOutputs() 获取有效设备。

  2. 使用 startCapture*() 开启采集, 采集到的资源将通过回调传递。

  3. 使用 stopCapture*() 停止采集。

---

### 许可证

   使用 `MIT LICENSE`

---

### 开发环境

  Windows 11，Qt 5.15.2
