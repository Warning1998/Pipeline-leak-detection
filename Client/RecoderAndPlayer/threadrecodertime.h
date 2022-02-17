#ifndef THREADRECODERTIME_H
#define THREADRECODERTIME_H

#include <QObject>
#include <QThread>
#include <QDebug>
class ThreadRecoderTime : public QObject
{
    Q_OBJECT
public:
    explicit ThreadRecoderTime(QObject *parent = nullptr);

    void xTimeout();                //计算时长
    void setFlag(bool flag = true); //修改flag
signals:
    void xTimeoutSignal();

private:
    bool isStop;
};

#endif // THREADRECODERTIME_H
