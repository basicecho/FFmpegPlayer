#ifndef FFMPEGPLAYER_H
#define FFMPEGPLAYER_H

#include <QObject>
extern "C"
{
    #include "libavformat/avformat.h"
    #include "libavcodec/avcodec.h"
}

class FFmpegPlayer : public QObject
{
    Q_OBJECT
public:
    explicit FFmpegPlayer(QObject *parent = nullptr);

signals:
    AVFormatContext *m_fmtCtx = nullptr;    // 视频上下文
    AVCodecContext *m_codecCtx = nullptr;   // 解码器上下文
};

#endif // FFMPEGPLAYER_H
