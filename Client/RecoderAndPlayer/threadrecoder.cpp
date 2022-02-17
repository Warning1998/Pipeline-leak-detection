#include "threadrecoder.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>      //添加file头文件
#include <QDateTime> //系统时间
#include <QAudioDeviceInfo>
#include "recoderandplayer.h"

QString RECORD_FILENAME = "/storage/emulated/0/UIDdate/";
QString RAW_RECORD_FILENAME; // 录音文件名;
ThreadRecoder::ThreadRecoder(QWidget *parent)
    : QMainWindow(parent), m_isRecord(false), m_isPlay(false)
{
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    qDebug() << "AudioDevice supportedCodecs : " << info.supportedCodecs();
}
// RecoderAudio作为线程处理函数，用于录制音频文件
//当mianwindow.cpp中的子线程  开始后，获得信号后跳转到该程序，实现音频的录制
void ThreadRecoder::RecoderAudio()
{ // 如果正在播放则停止播放;
    if (m_isPlay)
    {
        RecoderAudioStop();
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
            QString RawName = RecoderAndPlayer::wavName;
            RAW_RECORD_FILENAME = RECORD_FILENAME + RawName + ".raw";
            RecoderAndPlayer::WAV_RECORD_FILENAME = RECORD_FILENAME + RawName + ".wav";
            destinationFile.setFileName(RAW_RECORD_FILENAME);
            destinationFile.open(QIODevice::WriteOnly | QIODevice::Truncate);

            QAudioFormat format;                             // 设置音频文件格式;
            format.setSampleRate(44100);                     // 设置采样频率;
            format.setChannelCount(1);                       // 设置通道数;
            format.setSampleSize(16);                        // 设置每次采样得到的样本数据位值;
            format.setCodec("audio/pcm");                    // 设置编码方法;
            format.setByteOrder(QAudioFormat::LittleEndian); // 设置采样字节存储顺序;
            format.setSampleType(QAudioFormat::SignedInt);   // 设置采样类型;

            // 判断当前设备设置是否支持该音频格式;
            if (!audioDeviceInfo.isFormatSupported(format))
            {
                qDebug() << "Default format not supported, trying to use the nearest.";
                format = audioDeviceInfo.nearestFormat(format);
            }
            // 开始录音;
            m_audioInput = new QAudioInput(format, this);
            m_audioInput->start(&destinationFile);

            if (m_audioInput->state() == QAudio::ActiveState)
                emit Recording(); //转到主函数，显示了录音时长
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
void ThreadRecoder::RecoderAudioStop()
{ //停止
    qDebug() << "m_isRecord = true";
    if (m_isRecord)
    { // 初始化数据;
        m_isRecord = false;
        if (m_audioInput != NULL)
        {
            m_audioInput->stop();
            destinationFile.close();
            delete m_audioInput;
            m_audioInput = NULL;
        }
        // 将生成的.raw文件转成.wav格式文件;
        if (addWavHeader(RAW_RECORD_FILENAME, RecoderAndPlayer::WAV_RECORD_FILENAME) > 0)
            QMessageBox::information(NULL, tr("保存"), tr("音频录制结束"));
    }
    qDebug() << "stop record";
    emit RecordStopReady();
}
// 将生成的.raw文件转成.wav格式文件;
qint64 ThreadRecoder::addWavHeader(QString catheFileName, QString wavFileName)
{
    // 开始设置WAV的文件头
    WAVFILEHEADER WavFileHeader;
    qstrcpy(WavFileHeader.RiffName, "RIFF");
    qstrcpy(WavFileHeader.WavName, "WAVE");
    qstrcpy(WavFileHeader.FmtName, "fmt ");
    qstrcpy(WavFileHeader.DATANAME, "data");

    // 表示 FMT块 的长度
    WavFileHeader.nFmtLength = 16;
    // 表示 按照PCM 编码;
    WavFileHeader.nAudioFormat = 1;
    // 声道数目;
    WavFileHeader.nChannleNumber = 1;
    // 采样频率;
    WavFileHeader.nSampleRate = 44100;
    // nBytesPerSample 和 nBytesPerSecond这两个值通过设置的参数计算得到;
    // 数据块对齐单位(每个采样需要的字节数 = 通道数 × 每次采样得到的样本数据位数 / 8 )
    WavFileHeader.nBytesPerSample = 2;
    // 波形数据传输速率
    // (每秒平均字节数 = 采样频率 × 通道数 × 每次采样得到的样本数据位数 / 8  = 采样频率 × 每个采样需要的字节数 )
    WavFileHeader.nBytesPerSecond = 88200;
    // 每次采样得到的样本数据位数;
    WavFileHeader.nBitsPerSample = 16;

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

    WavFileHeader.nRiffLength = static_cast<unsigned long>(nFileLen - 8 + nSize);
    // static_cast<类型>(变量);  //新式的强制转换
    WavFileHeader.nDataLength = static_cast<unsigned long>(nFileLen);

    // 先将wav文件头信息写入，再将音频数据写入;
    wavFile.write((const char *)&WavFileHeader, nSize);
    wavFile.write(cacheFile.readAll());

    cacheFile.close();
    wavFile.close();

    return nFileLen;
}
