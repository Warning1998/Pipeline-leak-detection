#include "recoderandplayer.h"
#include "ui_recoderandplayer.h"
#include <QDateTime>

QString RecoderAndPlayer::wavName;             //当前录制音频文件的名称
QString RecoderAndPlayer::WAV_RECORD_FILENAME; // 录音文件转wav格式文件名;
static int iTime = 0;                          //录音时长
RecoderAndPlayer::RecoderAndPlayer(QWidget *parent) : QMainWindow(parent),
                                                      ui(new Ui::RecoderAndPlayer)
{
    ui->setupUi(this);
    xR = new ThreadRecoder;            //自定义类对象，分配空间，不指定父对象
    threadRecoder = new QThread(this); //创建子线程
    xR->moveToThread(threadRecoder);   //将自定义线程对象移到子线程中

    connect(this, &RecoderAndPlayer::destroyed, this, &RecoderAndPlayer::windowclose);          //当关闭界面时，需要关闭线程，释放动态空间,实际上没撒子用了，保留以防万一
    connect(this, &RecoderAndPlayer::startReconderThread, xR, &ThreadRecoder::RecoderAudio);    //已经开启录音线程，开始录音
    connect(this, &RecoderAndPlayer::stopReconderThread, xR, &ThreadRecoder::RecoderAudioStop); //停止录音
    connect(xR, &ThreadRecoder::RecordStopReady, this, &RecoderAndPlayer::stopRecordThread);    //停止录音线程

    xRT = new ThreadRecoderTime;           //自定义类对象，分配空间，不指定父对象
    threadRecoderTime = new QThread(this); //创建子线程
    xRT->moveToThread(threadRecoderTime);  //将自定义线程对象移到子线程中

    connect(xR, &ThreadRecoder::Recording, this, &RecoderAndPlayer::showRecordTime);              //打开显示录制时长线程
    connect(this, &RecoderAndPlayer::startReconderTimeThread, xRT, &ThreadRecoderTime::xTimeout); //已经开启录音线程，开始录音
    connect(xRT, &ThreadRecoderTime::xTimeoutSignal, this, &RecoderAndPlayer::dealsingal);
}

RecoderAndPlayer::~RecoderAndPlayer()
{
    delete ui;
}

void RecoderAndPlayer::on_RecReturnButton_clicked()
{ //隐藏此界面
    this->hide();
}
//当关闭界面时，需要关闭线程，释放动态空间
void RecoderAndPlayer::windowclose()
{
    stopRecordThread(); //停止录音线程
    stopRecordTime();
    delete xR;  //删除动态对象
    delete xRT; //删除动态对象
    // delete SAVEWAV;
}
////////////////////////////////////////////////////////////////////////////////////
///有关录音的槽函数
//点击按钮，开始录制音频文件，在此之前需要先检测录音设备
void RecoderAndPlayer::on_RecRecordButton_clicked()
{
    if (threadRecoder->isRunning() == true)
    {
        qDebug() << "threadRecoder isRunning 1";
        return;
    }
    threadRecoder->start();                                      //启动子线程，但是不启动线程处理函数
    iTime = 0;                                                   //录音时长归零
    QDateTime curDateTime = QDateTime::currentDateTime();        //获取当前时间
    RecoderAndPlayer::wavName = curDateTime.toString("yyMdHmm"); //设置时间格式
    emit startReconderThread();                                  //发送信号，转到xR的处理函数
}
//点击按钮停止录音，关闭线程
void RecoderAndPlayer::on_RecStopButton_clicked()
{
    if (threadRecoder->isRunning() == false)
    {
        qDebug() << "threadRecoder is not Running1";
        return;
    }
    stopRecordTime(); //停止显示时长
    emit stopReconderThread();
}
//停止录音线程
void RecoderAndPlayer::stopRecordThread()
{
    if (threadRecoder->isRunning() == false)
    {
        qDebug() << "threadRecoder is not Running2";
        return;
    }
    threadRecoder->quit();
    threadRecoder->wait();
    qDebug() << "stop thread";
}
////////////////////////////////////////////////////////////////////////////////////
///有关录音时长的槽函数
void RecoderAndPlayer::showRecordTime()
{
    if (threadRecoderTime->isRunning() == true)
    {
        qDebug() << "threadRecoder->isRunning() == true";
        return;
    }
    threadRecoderTime->start(); //启动子线程，但是不启动线程处理函数
    xRT->setFlag(false);        //启动处理函数
    qDebug() << "111";
    emit startReconderTimeThread(); //发送信号，转到xRT的处理函数
    ui->label_2->setText("正在录音的音频文件名称:" + RecoderAndPlayer::wavName + ".wav");
}
void RecoderAndPlayer::dealsingal()
{
    iTime++;
    ui->lcdNumber->display(iTime);
}
void RecoderAndPlayer::stopRecordTime()
{
    if (threadRecoderTime->isRunning() == false)
    {
        qDebug() << "11111111";
        return;
    }
    xRT->setFlag(true);
    threadRecoderTime->quit();
    threadRecoderTime->wait();
    ui->label_2->setText("音频文件（" + RecoderAndPlayer::wavName + ".wav） 录音结束");
}
