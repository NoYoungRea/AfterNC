#include "BinaryBuffer.h"

BinaryBuffer::BinaryBuffer()
{
    m_UseSize = 0;
    m_pEndBuffer = m_Buffer + BUFFER_LENGTH;
    m_pStartCursor = m_Buffer;
    m_pEndCursor = m_Buffer;
}

int BinaryBuffer::GetUseSize()
{
    return m_UseSize;
}

bool BinaryBuffer::PeekData(BYTE* byDest, int size)
{
    if (m_UseSize < size)
    {
        return false;
    }
    memcpy(byDest, m_pStartCursor, size);
    return true;
}

bool BinaryBuffer::GetData(BYTE* byDest, int size)
{
    if (!PeekData(byDest, size))
    {
        return false;
    }

    m_UseSize -= size;
    m_pStartCursor += size;
    return true;
}

bool BinaryBuffer::SystemPutData(int size)
{
    m_pEndCursor += size;
    m_UseSize += size;
    if (m_pEndCursor > m_pEndBuffer)
    {
        return false;
    }
    return true;
}

void BinaryBuffer::MoveStartCursor(int size)
{
    m_pStartCursor += size;
    m_UseSize -= size;
}

void BinaryBuffer::MoveRemainingData()
{
    memmove(m_Buffer, m_pStartCursor, m_UseSize);
    m_pStartCursor = m_Buffer;
    m_pEndCursor = m_Buffer + m_UseSize;
}
