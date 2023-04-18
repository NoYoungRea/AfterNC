#pragma once
#define RING_BUFFER_SIZE 1024
#include <functional>

class BinaryRingBuffer
{
public:
    char* GetBeginOfBuffer();
    size_t DataSize();
    size_t EmptySize();
    size_t ContinuesDataSize();
    size_t ContinuesEmptySize();
    int PutData(char* srcData, int size);
    int GetData(char* destData, int size);
    size_t _DataSize(const int additinalDataSize = 0);
    size_t EmptySize(const bool toEndOfBuffer = false);
    

    inline bool IsForward()
    {
        //같은 경우는 데이터가 없을때, 데이터가 다 차도 둘이 만날 수 없음(한칸 이격거리)
        return m_BeginCursor <= m_EndCursor; 
    }
private:
    
    static const int ActualRingBufferSize = RING_BUFFER_SIZE + 1;
    char m_BeginOfBuffer[ActualRingBufferSize];
    char* m_EndOfBuffer{ m_BeginOfBuffer + ActualRingBufferSize };
    char* m_BeginCursor{ m_BeginOfBuffer };
    char* m_EndCursor{ m_BeginOfBuffer };
};
