
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>      //添加file头文件
#include <QDateTime> //系统时间
#include <QAudioDeviceInfo>

const qint64 TIME_TRANSFORM = 1000 * 1000; // 微妙转秒;
QString RAW_RECORD_FILENAME= "/storage/emulated/0/UIDdate/1.raw";// 录音文件名;
QString WAV_RECORD_FILENAME= "/storage/emulated/0/UIDdate/1.wav";// 录音文件转wav格式文件名;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_isRecord(false), m_isPlay(false), m_RecordTimerId(0), m_RecordTime(0)
{
    ui->setupUi(this);
    // 输出当前设备支持的音频编码格式;
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    qDebug() << "AudioDevice supportedCodecs : " << info.supportedCodecs();

    player = new QMediaPlayer(this);//
    playlist = new QMediaPlaylist(this);//
    playlist->setPlaybackMode(QMediaPlaylist::CurrentItemOnce); //循环模式

    player->setPlaylist(playlist);
}

MainWindow::~MainWindow()
{
    delete ui;
}

///录制音频
void MainWindow::on_RecordButton_clicked()
{    // 如果正在播放则停止播放;
    if (m_isPlay)
    {
        on_StopButton_clicked();
        QMessageBox::information(NULL, tr("Record"), tr("stop"));
    }
    // 如果当前没有开始录音则允许录音;
    if (!m_isRecord)
    {
        // 判断本地设备是否支持该格式
        QAudioDeviceInfo audioDeviceInfo = QAudioDeviceInfo::defaultInputDevice();
        // 判断本地是否有录音设备;
        if (!audioDeviceInfo.isNull())
        {
            m_isRecord = true;
            destinationFile.setFileName(RAW_RECORD_FILENAME);
            destinationFile.open(QIODevice::WriteOnly | QIODevice::Truncate);

            QAudioFormat format;                             // 设置音频文件格式;
            format.setSampleRate(8000);                     // 设置采样频率;
            format.setChannelCount(1);                       // 设置通道数;
            format.setSampleSize(16);                        // 设置每次采样得到的样本数据位值;
            format.setCodec("audio/pcm");                    // 设置编码方法;
            format.setByteOrder(QAudioFormat::LittleEndian); // 设置采样字节存储顺序;
            format.setSampleType(QAudioFormat::UnSignedInt); // 设置采样类型;

            // 判断当前设备设置是否支持该音频格式;
            if (!audioDeviceInfo.isFormatSupported(format))
            {
                qDebug() << "Default format not supported, trying to use the nearest.";
                format = audioDeviceInfo.nearestFormat(format);
            }
            // 开始录音;
            m_audioInput = new QAudioInput(format, this);
            m_audioInput->start(&destinationFile);

            // 开启时钟，用于更新当前录音时间;
            if (m_RecordTimerId == 0)
            {
                m_RecordTimerId = startTimer(100);
            }
        }
        else
        { // 没有录音设备;
            QMessageBox::information(NULL, tr("Record"), tr("Current No Record Device"));
        }
    }
    else
    { // 当前正在录音;
        QMessageBox::information(NULL, tr("Record"), tr("Current is Recording"));
    }
}

void MainWindow::on_StopButton_clicked()
{ //停止
    if (m_isRecord)
    {
        // 关闭时钟，并初始化数据;
        killTimer(m_RecordTimerId);
        m_RecordTime = 0;
        m_RecordTimerId = 0;
        m_isRecord = false;
        ui->LabPassTime->setText(QString("Idle : %1/S").arg(m_RecordTime));

        if (m_audioInput != NULL)
        {
            m_audioInput->stop();
            destinationFile.close();
            delete m_audioInput;
            m_audioInput = NULL;
        }
        // 将生成的.raw文件转成.wav格式文件;
         if (addWavHeader(RAW_RECORD_FILENAME, WAV_RECORD_FILENAME) > 0)
            QMessageBox::information(NULL, tr("Save"), tr("RecordFile Save Success"));
    }
}
// 时钟事件,更新状态;
void MainWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_RecordTimerId)
    {
        QString strState;
        if (m_isRecord)
        {
            strState = "Recording";
            m_RecordTime = m_audioInput->elapsedUSecs() / TIME_TRANSFORM;
        }
        else if (m_isPlay)
        {
            strState = "Playing";
            m_RecordTime = m_audioOutput->elapsedUSecs() / TIME_TRANSFORM;
        }

        ui->LabPassTime->setText(QString("%1 : %2/S").arg(strState).arg(m_RecordTime));
    }
}
// 将生成的.raw文件转成.wav格式文件;
qint64 MainWindow::addWavHeader(QString catheFileName, QString wavFileName)
{
    // 开始准备WAV的文件头
    WAVFILEHEADER WavFileHeader;
    qstrcpy(WavFileHeader.RiffName, "RIFF");
    qstrcpy(WavFileHeader.WavName, "WAVE");
    qstrcpy(WavFileHeader.FmtName, "fmt ");

    WavFileHeader.nFmtLength = 16;	//  表示 FMT 的长度
    WavFileHeader.nAudioFormat = 1; //这个表示 PCM 编码;

    qstrcpy(WavFileHeader.DATANAME, "data");

    WavFileHeader.nBitsPerSample = 16;
    WavFileHeader.nBytesPerSample = 2;
    WavFileHeader.nSampleRate = 44100;
    WavFileHeader.nBytesPerSecond = 88200;
    WavFileHeader.nChannleNumber = 1;

    QFile cacheFile(catheFileName);
    QFile wavFile(wavFileName);

    if (!cacheFile.open(QIODevice::ReadWrite))
    {
        return -1;
    }
    if (!wavFile.open(QIODevice::WriteOnly))
    {
        return -2;
    }

    int nSize = sizeof(WavFileHeader);
    qint64 nFileLen = cacheFile.bytesAvailable();

    WavFileHeader.nRiffLength = nFileLen - 8 + nSize;
    WavFileHeader.nDataLength = nFileLen;

    // 先将wav文件头信息写入，再将音频数据写入;
    wavFile.write((char *)&WavFileHeader, nSize);
    wavFile.write(cacheFile.readAll());

    cacheFile.close();
    wavFile.close();

    return nFileLen;
}



void MainWindow::on_PlayButton_clicked()
{
    playlist->addMedia(QUrl::fromLocalFile(WAV_RECORD_FILENAME));//添加文件
    if (player->state()!=QMediaPlayer::PlayingState)
        playlist->setCurrentIndex(0);
    player->play();
}


void MainWindow::on_StopPlayButton_clicked()
{
    player->stop();
}

