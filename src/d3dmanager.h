#ifndef D3DMANAGER_H
#define D3DMANAGER_H

#include <QScopedPointer>

#ifdef Q_OS_WIN

#include <QPixmap>
#include <QScreen>

QT_FORWARD_DECLARE_CLASS(D3dManagerPrivate);

class D3dManager
{
public:
    D3dManager();
    ~D3dManager();

    /**
     * @brief 初始化D3D
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
    QScopedPointer<D3dManagerPrivate> d_ptr;
    Q_DECLARE_PRIVATE(D3dManager);
};

#endif

#endif // D3DMANAGER_H
