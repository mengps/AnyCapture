#include "camerasurface.h"
#include "yuvconvertor.h"

#include <QDebug>
#include <QVideoSurfaceFormat>

using namespace any_capture;

class CameraSurfacePrivate
{
public:
    capture_callback m_callback;
    void *m_master = nullptr;
    QVideoFrame::PixelFormat m_pixelFormat;
    OutputVideoFormat m_outFormat = OutputVideoFormat::Format_RGB24;
    QVideoFrame m_currentFrame;
    QVector<uchar> m_convertCache;
};

CameraSurface::CameraSurface(QObject *parent)
    : QAbstractVideoSurface(parent)
    , d_ptr(new CameraSurfacePrivate)
{

}

CameraSurface::~CameraSurface()
{

}

void CameraSurface::setSurfaceFormat(OutputVideoFormat format, capture_callback callback, void *master)
{
    Q_D(CameraSurface);

    d->m_outFormat = format;
    d->m_callback = callback;
    d->m_master = master;
}

QVideoFrame CameraSurface::grabFrame() const
{
    Q_D(const CameraSurface);

    return d->m_currentFrame;
}

bool CameraSurface::start(const QVideoSurfaceFormat &format)
{
    Q_D(CameraSurface);

    bool success = QAbstractVideoSurface::start(format);
    if (success) {
        d->m_pixelFormat = format.pixelFormat();
        if (d->m_outFormat == OutputVideoFormat::Format_RGB24) {
            d->m_convertCache.resize(format.frameWidth() * format.frameHeight() * 3);
        } else if (d->m_outFormat == OutputVideoFormat::Format_YUV420P) {
            d->m_convertCache.resize(format.frameWidth() * format.frameHeight() * 1.5);
        }
    }

    return success;
}

QList<QVideoFrame::PixelFormat> CameraSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType) const
{
    return QList<QVideoFrame::PixelFormat>()
            << QVideoFrame::Format_ARGB32
            << QVideoFrame::Format_ARGB32_Premultiplied
            << QVideoFrame::Format_RGB32
            << QVideoFrame::Format_RGB24
            << QVideoFrame::Format_RGB565
            << QVideoFrame::Format_RGB555
            << QVideoFrame::Format_ARGB8565_Premultiplied
            << QVideoFrame::Format_BGRA32
            << QVideoFrame::Format_BGRA32_Premultiplied
            << QVideoFrame::Format_BGR32
            << QVideoFrame::Format_BGR24
            << QVideoFrame::Format_BGR565
            << QVideoFrame::Format_BGR555
            << QVideoFrame::Format_BGRA5658_Premultiplied
            << QVideoFrame::Format_AYUV444
            << QVideoFrame::Format_AYUV444_Premultiplied
            << QVideoFrame::Format_YUV444
            << QVideoFrame::Format_YUV420P
            << QVideoFrame::Format_YV12
            << QVideoFrame::Format_UYVY
            << QVideoFrame::Format_YUYV
            << QVideoFrame::Format_NV12
            << QVideoFrame::Format_NV21
            << QVideoFrame::Format_IMC1
            << QVideoFrame::Format_IMC2
            << QVideoFrame::Format_IMC3
            << QVideoFrame::Format_IMC4
            << QVideoFrame::Format_Y8
            << QVideoFrame::Format_Y16
            << QVideoFrame::Format_Jpeg
            << QVideoFrame::Format_CameraRaw
            << QVideoFrame::Format_AdobeDng;
}

bool CameraSurface::present(const QVideoFrame &frame)
{
    Q_D(CameraSurface);

    if (frame.isValid()) {
        d->m_currentFrame = frame;
        if (d->m_callback) {
            d->m_currentFrame.map(QAbstractVideoBuffer::ReadOnly);
            if (d->m_currentFrame.map(QAbstractVideoBuffer::ReadOnly)) {
                /*! 输出RGB24 */
                if (d->m_outFormat == OutputVideoFormat::Format_RGB24) {
                    if (d->m_currentFrame.pixelFormat() == QVideoFrame::Format_RGB24) {
                        if (d->m_currentFrame.map(QAbstractVideoBuffer::ReadOnly)) {
                            d->m_callback((uchar *)d->m_currentFrame.bits(), d->m_currentFrame.mappedBytes(), d->m_master);
                            d->m_currentFrame.unmap();
                        }
                    } else {
                        QImage image = d->m_currentFrame.image();
                        if (image.format() != QImage::Format_RGB888)
                            image = image.convertToFormat(QImage::Format_RGB888);
                        d->m_callback((uchar *)image.constBits(), image.sizeInBytes(), d->m_master);
                    }
                } else if (d->m_outFormat == OutputVideoFormat::Format_YUV420P) {
                    /*! 输出YUV420P */
                    auto frameWidth = surfaceFormat().frameWidth();
                    auto frameHeight = surfaceFormat().frameHeight();
                    switch (d->m_pixelFormat) {
                    case QVideoFrame::PixelFormat::Format_YUYV:
                        /*! YUYV->YUV420P */
                        YUVConvertor::yuy2ToI420(d->m_currentFrame.bits()
                                                 , frameWidth, frameHeight
                                                 , d->m_convertCache.data());
                        d->m_callback(d->m_convertCache.data(), d->m_convertCache.size(), d->m_master);
                        break;
                    case QVideoFrame::PixelFormat::Format_Jpeg:
                    {
                        /*! JPEG->RGB32->YUV420P */
                        QImage image = d->m_currentFrame.image();
                        if (image.format() != QImage::Format_RGB888) {
                            image = image.convertToFormat(QImage::Format_RGB32);
                        }
                        YUVConvertor::argb32ToI420((uchar *)image.constBits()
                                                   , frameWidth, frameHeight
                                                   , d->m_convertCache.data());
                        d->m_callback(d->m_convertCache.data(), d->m_convertCache.size(), d->m_master);
                    } break;
                    default:
                        break;
                    }
                }
            }
        }
    }

    return false;
}
