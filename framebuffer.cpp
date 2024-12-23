#include "framebuffer.h"
#include <iostream>
#include <string.h>

FrameBuffer::FrameBuffer()
{

}

FrameBuffer::~FrameBuffer()
{
    if(m_dataBuf)
    {
        delete m_dataBuf;
        m_dataBuf = nullptr;
    }
}

void FrameBuffer::initBuffer(int capacity)
{
    if(m_dataBuf)
    {
        delete m_dataBuf;
        m_dataBuf = nullptr;
    }

    m_dataBuf = new uint8_t[capacity];
    m_dataCapacity = capacity;
    m_dataLen = 0;
    m_dataReadPtr = 0;
    m_dataWritePtr = 0;
}

void FrameBuffer::enqueue(int frameType, int64_t pts, int64_t dts, uint8_t *data, int size)
{
    if(m_dataCapacity < size)
    {
        std::cout << "frameBuffer is to small, size = " << m_dataCapacity << ",frameSize = " << size << std::endl;
        return;
    }

    std::unique_lock<std::mutex> lock(m_mtx);
    if(m_dataCapacity - m_dataLen < size)
    {
        m_cv.wait(lock, [this, size](){ return m_dataCapacity - m_dataLen >= size; });
    }

    int endPos = m_dataWritePtr + size;
    if(endPos > m_dataCapacity)
    {
        memcpy(m_dataBuf + m_dataWritePtr, data, m_dataCapacity - m_dataWritePtr);
        memcpy(m_dataBuf, data, endPos % m_dataCapacity);
    }
    else
    {
        memcpy(m_dataBuf + m_dataWritePtr, data, size);
    }

    MyFrame frame{m_dataWritePtr, size, frameType, pts, dts};
    m_frameQueue.push(frame);
    m_dataWritePtr = endPos % m_dataCapacity;
    m_dataLen += size;

    m_cv.notify_one();
}

void FrameBuffer::dequeue(int &frameType, int64_t pts, int64_t dts, uint8_t **data, int &size)
{
    std::unique_lock<std::mutex> lock(m_mtx);
    if(m_frameQueue.empty())
    {
        m_cv.wait(lock, [this](){ return !m_frameQueue.empty(); });
    }

    MyFrame frame = m_frameQueue.front();

    frameType = frame.frameType;
    pts = frame.pts;
    dts = frame.dts;
    *data = new uint8_t[frame.bufSize];
    size = frame.bufSize;

    int endPos = m_dataWritePtr + size;
    if(endPos > m_dataCapacity)
    {
        memcpy(data, m_dataBuf + m_dataWritePtr, m_dataCapacity - m_dataWritePtr);
        memcpy(data + (m_dataCapacity - m_dataWritePtr), m_dataBuf, endPos % m_dataCapacity);
    }
    else
    {
        memcpy(data, m_dataBuf + m_dataWritePtr, size);
    }

    m_frameQueue.pop();
    m_dataWritePtr = endPos % m_dataCapacity;
    m_dataLen -= size;

    m_cv.notify_one();
}

void FrameBuffer::remove()
{
    std::unique_lock<std::mutex> lock(m_mtx);
    if(m_frameQueue.empty())
    {
        return;
    }

    MyFrame frame = m_frameQueue.front();
    m_frameQueue.pop();
    m_dataWritePtr = (m_dataWritePtr + frame.bufSize) % m_dataCapacity;
    m_dataLen -= frame.bufSize;
}

void FrameBuffer::clear()
{
    std::unique_lock<std::mutex> lock(m_mtx);
    while(!m_frameQueue.empty())
    {
        m_frameQueue.pop();
    }

    m_dataLen = 0;
    m_dataReadPtr = 0;
    m_dataWritePtr = 0;
}

int FrameBuffer::getQueueSize()
{
    std::unique_lock<std::mutex> lock(m_mtx);
    return m_frameQueue.size();
}

int FrameBuffer::getFreeLength()
{
    std::unique_lock<std::mutex> lock(m_mtx);
    return m_dataCapacity - m_dataLen;
}
