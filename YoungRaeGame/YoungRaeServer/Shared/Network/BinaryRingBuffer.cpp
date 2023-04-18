#include "BinaryRingBuffer.h"


char* BinaryRingBuffer::GetBeginOfBuffer()
{
    return m_BeginOfBuffer;
}

size_t BinaryRingBuffer::DataSize(const bool toEndOfBuffer)
{
    if (IsForward())
    {
        return m_EndCursor - m_BeginCursor;
    }

    return (m_EndOfBuffer - m_BeginCursor) + 
        toEndOfBuffer ? 0 : (m_EndCursor - m_BeginOfBuffer);
}


size_t BinaryRingBuffer::EmptySize(const bool toEndOfBuffer)
{
    if (!IsForward())
    {
        //항상 begincusro 전 한칸은 비워놔야 한다.
        return (m_BeginCursor - 1) - m_EndCursor;
    }

    if (m_BeginOfBuffer == m_BeginCursor)
    {
        return (m_EndOfBuffer - 1) - m_EndCursor;
    }

    return (m_EndOfBuffer - m_EndCursor) +
        toEndOfBuffer ? 0 : (m_BeginCursor - 1) - m_BeginOfBuffer;
}

int BinaryRingBuffer::PutData(char* srcData, int size)
{
}

int BinaryRingBuffer::GetData(char* destData, int size)
{
    DataSize(false)

    const auto DataSizeToEndOfBuffer = DataSize(true);
    if ( >)

}

int BinaryRingBuffer::GetData(protobuf* msg, int size)
{
    if (DataSize(true) >= size)
    {

    }
    msg.ParseFromArray()

}


