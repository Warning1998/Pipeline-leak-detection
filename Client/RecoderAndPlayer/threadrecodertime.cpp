#include "threadrecodertime.h"

ThreadRecoderTime::ThreadRecoderTime(QObject *parent)
    : QObject{parent}
{
    isStop = false;
}
void ThreadRecoderTime::xTimeout()
{
    while (isStop == false)
    {
        QThread::sleep(1);
        emit xTimeoutSignal();
        if (true == isStop)
            break;
    }
}

void ThreadRecoderTime::setFlag(bool flag)
{
    isStop = flag;
}
