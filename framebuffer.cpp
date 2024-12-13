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
    m_dataBuf = new uint8_t[capacity];

    m_dataCapacity = capacity;
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
}

void FrameBuffer::dequeue(int &frameType, int64_t pts, int64_t dts, uint8_t **data, int &size)
{
    std::unique_lock<std::mutex> lock(m_mtx);
    if(m_frameQueue.empty())
    {
        m_cv.wait(lock, [this](){ return !m_frameQueue.empty(); });
    }
}

//void FrameBuffer::enqueue(MyFrame frame, uint8_t *data, int size)
//{
//    if(m_dataCapacity < size)
//    {
//        std::cout << "缓冲区容量太小，没有这么多空间可以存放" << std::endl;
//        return;
//    }

//    std::unique_lock<std::mutex> lock(m_mtx);
//    if(m_dataCapacity - m_dataLen < size)
//    {
//        m_cv.wait(lock, [this, size](){ return m_dataCapacity - m_dataLen >= size; });
//    }

//    int endPos = m_dataWritePtr + size;
//    if(endPos > m_dataCapacity)
//    {
//        memcpy(m_dataBuf + m_dataWritePtr, data, m_dataCapacity - m_dataWritePtr);
//        memcpy(m_dataBuf, data, endPos % m_dataCapacity);
//    }
//    else
//    {
//        memcpy(m_dataBuf + m_dataWritePtr, data, size);
//    }

//    m_frameQueue.push(frame);
//    m_dataWritePtr = endPos % m_dataCapacity;
//}

//void FrameBuffer::dequeue(MyFrame frame, uint8_t **data, int &size)
//{
//    std::unique_lock<std::mutex> lock(m_mtx);
//    if(m_dataLen < size)
//    {
//        m_cv.wait(lock, [this, size](){ return m_dataLen >= size; });
//    }

//    int endPos = m_dataReadPtr + size;
//    if(endPos > m_dataCapacity)
//    {
//        memcpy(data, m_dataBuf + m_dataReadPtr, m_dataCapacity - m_dataReadPtr);
//        memcpy(data, m_dataBuf, endPos % m_dataCapacity);
//    }
//    else
//    {
//        memcpy(data, m_dataBuf + m_dataReadPtr, size);
//    }

//    m_dataReadPtr = endPos % m_dataCapacity;
//}

void FrameBuffer::clear()
{
    m_dataLen = 0;
    m_dataReadPtr = 0;
    m_dataWritePtr = 0;
}

int FrameBuffer::getFreeLength()
{
    std::unique_lock<std::mutex> lock(m_mtx);
    return m_dataCapacity - m_dataLen;
}
