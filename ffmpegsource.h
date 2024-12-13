#ifndef FFMPEGSOURCE_H
#define FFMPEGSOURCE_H

#include "framebuffer.h"

#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>

class AVFormatContext;
class AVStream;
class AVCodecContext;
class AVCodec;
class AVFrame;
class AVPacket;
class SwsContext;

/*
1. 每次从avformat中读取的，视频帧和音频帧是不确定的，要进行stream_index进行区分
2. 采用环形缓冲区读取数据，读和写可以分开进行
*/

enum class FrameType
{
    AUDIO_FRAME = 0,
    VIDEO_FRAME = 1,
};

//struct MyFrame
//{
//    std::vector<uint8_t> data;
//    int size;
//    FrameType type;
//    int64_t pts;
//    int64_t dts;
//};

#define BUFFER_SIZE 10

class FFMpegSource
{
public:
    static void startDemux(void *arg);
    static void startDecoderAudio(void *arg);
    static void startDecoderVideo(void *arg);
public:
    FFMpegSource();
    void initFFmpegSource(std::string inputFile);
    void initDecoder();
    void worker();

    void Demux();
    void DecoderAudio();
    void DecoderVideo();

    bool waitStop(int durationMS);



private:
    // 视频的基本信息
    std::string m_inputFile;

    int m_durationHH;
    int m_durationMM;
    int m_durationSS;

    int m_frameRate;
    int m_frameWidth;
    int m_frameHeight;

    // 线程的控制变量
    bool m_stopFlag = false;
    std::thread m_pThread;
    std::mutex m_stopMtx;
    std::condition_variable m_stopCV;

    // ffmpeg上下文相关信息
    AVFormatContext *m_pFmtCtx = nullptr;
    AVStream *m_pAudioStream = nullptr;
    AVStream *m_pVideoStream = nullptr;
    AVPacket *m_pkt = nullptr;

    int m_audioIndex = -1;
    int m_videoIndex = -1;

    // audio编解码相关信息
    AVCodecContext *m_pAudioCodecCtx = nullptr;
    const AVCodec *m_pAudioCodec = nullptr;
    AVFrame *m_pAudioFrame = nullptr;
    AVPacket *m_pAudioPkt = nullptr;
    FrameBuffer m_audioPacketBuffer;
    FrameBuffer m_audioFrameBuffer;

    // video编解码相关信息
    AVCodecContext *m_pVideoCodecCtx = nullptr;
    const AVCodec *m_pVideoCodec = nullptr;
    AVFrame *m_pVideoFrame = nullptr;
    AVPacket *m_pVideoPkt = nullptr;
    SwsContext *m_pSwsCtx = nullptr;
    FrameBuffer m_videoPacketBuffer;
    FrameBuffer m_videoFrameBuffer;

};

#endif // FFMPEGSOURCE_H
