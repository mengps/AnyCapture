#ifndef ABSTRACTCAPTURETHREAD_H
#define ABSTRACTCAPTURETHREAD_H

#include <QThread>

class AbstractCaptureThread : public QThread
{
public:
    AbstractCaptureThread(QObject *parent = nullptr) : QThread(parent) { }

    virtual void startCapture();
    virtual void stopCapture();

protected:
    bool m_captrueThreadRunning = false;
};

#endif // ABSTRACTCAPTURETHREAD_H
