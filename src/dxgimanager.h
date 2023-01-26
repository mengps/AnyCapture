#ifndef DXGIMANAGER_H
#define DXGIMANAGER_H

#include <QScopedPointer>

#ifdef Q_OS_WIN

#include <QPixmap>
#include <QScreen>

QT_FORWARD_DECLARE_CLASS(DxgiManagerPrivate);

class DxgiManager
{
public:
    DxgiManager();
    ~DxgiManager();

    /**
     * @brief 初始化DXGI
     * @param 指定屏幕
     * @return bool{true:成功; false:失败}
     */
    bool initialize(QScreen *screen);

    /**
     * @brief 返回最后出现的错误字符串
     * @return QString
     */
    QString lastError() const;

    /**
     * @brief 捕获一帧屏幕图像
     * @return QImage
     */
    QImage grabScreen();

private:
    QScopedPointer<DxgiManagerPrivate> d_ptr;
    Q_DECLARE_PRIVATE(DxgiManager);
};

#endif

#endif // DXGIMANAGER_H
