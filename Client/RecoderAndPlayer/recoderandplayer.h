#ifndef RECODERANDPLAYER_H
#define RECODERANDPLAYER_H

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
QT_CHARTS_USE_NAMESPACE

#include <QThread>
#include "threadrecoder.h"
#include "threadrecodertime.h"
namespace Ui
{
    class RecoderAndPlayer;
}

class RecoderAndPlayer : public QMainWindow
{
    Q_OBJECT

public:
    explicit RecoderAndPlayer(QWidget *parent = nullptr);
    ~RecoderAndPlayer();
    void windowclose();
    static QString wavName;             //当前录制的音频文件的名称
    static QString WAV_RECORD_FILENAME; // 录音文件转wav格式文件名;
private slots:
    void on_RecReturnButton_clicked();
    void on_RecRecordButton_clicked();
    void on_RecStopButton_clicked();
    void stopRecordThread(); //停止录音线程
    void dealsingal();       //在ui中显示录音时长
    void showRecordTime();
    void stopRecordTime(); //停止显示录音时长

signals:
    void startReconderThread();     //启动录音信号
    void stopReconderThread();      //结束录音信号
    void startReconderTimeThread(); //启动录音时长信号

private:
    Ui::RecoderAndPlayer *ui;
    QThread *threadRecoder; //子线程：录制声音
    ThreadRecoder *xR;

    QThread *threadRecoderTime; //子线程：录制声音时长
    ThreadRecoderTime *xRT;
};

#endif // RECODERANDPLAYER_H
