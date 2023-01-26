#include "abstractcapturethread.h"

void AbstractCaptureThread::startCapture()
{
    m_captrueThreadRunning = true;
    QThread::start();
}

void AbstractCaptureThread::stopCapture()
{
    m_captrueThreadRunning = false;
    QThread::quit();
    QThread::wait();
}
