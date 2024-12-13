#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <queue>
#include <mutex>
#include <condition_variable>

// 一个环形缓冲区，每次分配完成后，进行一个入展
class FrameBuffer
{
public:
    FrameBuffer();
    ~FrameBuffer();
    void initBuffer(int capacity);
    // 插入数据时，0表示视频帧，1表示音频帧
    void enqueue(int frameType, int64_t pts, int64_t dts, uint8_t *data, int size); // 每次入栈和出栈都是一帧
    void dequeue(int &frameType, int64_t pts, int64_t dts, uint8_t **data, int &size);
    void remove();
    void clear();
    int getQueueSize();
    int getFreeLength();

private:
    struct MyFrame
    {
        int bufPos;
        int bufSize;
        int frameType; // 0表示音频帧，1表示视频帧
        int64_t pts;
        int64_t dts;
    };

private:
    std::mutex m_mtx;
    std::condition_variable m_cv;
    std::queue<MyFrame> m_frameQueue;

    uint8_t *m_dataBuf = nullptr;
    int m_dataCapacity = 0;
    int m_dataLen = 0;
    int m_dataReadPtr = 0;
    int m_dataWritePtr = 0;
};

#endif // FRAMEBUFFER_H
