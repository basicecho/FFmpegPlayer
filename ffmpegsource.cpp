#include "ffmpegsource.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
}

#include <chrono>

void FFMpegSource::startDemux(void *arg)
{
    FFMpegSource *source = (FFMpegSource *)arg;
    source->Demux();
}

void FFMpegSource::startDecoderAudio(void *arg)
{
    FFMpegSource *source = (FFMpegSource *)arg;
    source->DecoderAudio();
}

void FFMpegSource::startDecoderVideo(void *arg)
{
    FFMpegSource *source = (FFMpegSource *)arg;
    source->DecoderVideo();
}

FFMpegSource::FFMpegSource()
{

}

void FFMpegSource::initFFmpegSource(std::string inputFile)
{
    int ret = 0;

    m_inputFile = inputFile;

    ret = avformat_open_input(&m_pFmtCtx, m_inputFile.c_str(), nullptr, nullptr);
    if(ret < 0)
    {
        return;
    }

    ret = avformat_find_stream_info(m_pFmtCtx, nullptr);
    if(ret < 0)
    {
        return;
    }

    m_audioIndex = av_find_best_stream(m_pFmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    m_videoIndex = av_find_best_stream(m_pFmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if(m_audioIndex >= 0)
    {
        m_pAudioStream = m_pFmtCtx->streams[m_audioIndex];
    }
    if(m_videoIndex >= 0)
    {
        m_pVideoStream = m_pFmtCtx->streams[m_videoIndex];

        int64_t duration = m_pFmtCtx->duration / 1000000;
        m_durationHH = duration / 3600;
        m_durationMM = duration / 3600 / 60;
        m_durationSS = duration / 3600 % 60;

        m_frameWidth = m_pVideoStream->codecpar->width;
        m_frameHeight = m_pVideoStream->codecpar->height;
        m_frameRate = av_q2d(m_pVideoStream->avg_frame_rate);
    }
}

void FFMpegSource::initDecoder()
{
    int ret = 0;

    if(m_audioIndex >= 0)
    {

    }
    if(m_videoIndex >= 0)
    {
        m_pVideoCodecCtx = avcodec_alloc_context3(nullptr);
        ret = avcodec_parameters_to_context(m_pVideoCodecCtx, m_pVideoStream->codecpar);
        if(ret < 0)
        {
            return;
        }

        m_pVideoCodec = avcodec_find_decoder(m_pVideoCodecCtx->codec_id);
        ret = avcodec_open2(m_pVideoCodecCtx, m_pVideoCodec, nullptr);
        if(ret < 0)
        {
            return;
        }
    }
}

void FFMpegSource::worker()
{
    int ret = 0;



    // 启动视频和音频两个线程
}

void FFMpegSource::Demux()
{
    int ret = 0;
    m_pkt = av_packet_alloc();

    while(!waitStop(0))
    {
        ret = av_read_frame(m_pFmtCtx, m_pkt);
        if(ret < 0)
        {
            continue;
        }

        if(m_pkt->stream_index == m_audioIndex)
        {
            m_audioPacketBuffer.enqueue(m_pkt);
        }
        else if(m_pkt->stream_index == m_videoIndex)
        {
            m_videoPacketBuffer.enqueue(m_pkt);
        }
        else
        {
            av_packet_unref(m_pkt);
        }
    }
}

void FFMpegSource::DecoderAudio()
{

}

void FFMpegSource::DecoderVideo()
{
    int ret = 0;
    m_pVideoPkt = av_packet_alloc();
    m_pVideoFrame = av_frame_alloc();
    m_pSwsCtx = sws_getContext(m_frameWidth, m_frameHeight, AV_PIX_FMT_YUV420P,
                               m_frameWidth, m_frameHeight, AV_PIX_FMT_RGB24,
                               SWS_BILINEAR, nullptr, nullptr, nullptr);

    while(ret >= 0 && !waitStop(0))
    {
        m_videoPacketBuffer.dequeue(m_pVideoPkt);
        ret = avcodec_send_packet(m_pVideoCodecCtx, m_pVideoPkt);
        if(ret < 0)
        {
            continue;
        }

        while(true)
        {
            ret = avcodec_receive_frame(m_pVideoCodecCtx, m_pVideoFrame);
            if(ret < 0)
            {
                if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                {
                    continue;
                }
                else
                {
                    break;
                }
            }

            MyFrame *videoFrame = new MyFrame();
            videoFrame->data.resize(m_frameWidth * m_frameHeight * 3);
            uint8_t *dstSlice[4] = {videoFrame->data.data()};
            int dstLine[4] = {m_frameWidth * 3};

            sws_scale(m_pSwsCtx, m_pVideoFrame->data, m_pVideoFrame->linesize, 0, m_pVideoFrame->height, dstSlice, dstLine);
            m_videoFrameBuffer.enqueue(videoFrame);
            // 我要进行数据转换，所以需要一个东西来存储数据
        }
    }
}

bool FFMpegSource::waitStop(int durationMS)
{
    std::unique_lock<std::mutex> lock(m_stopMtx);
    m_stopCV.wait_for(lock, std::chrono::milliseconds(durationMS), [this]{return m_stopFlag;});
    return m_stopFlag;
}
