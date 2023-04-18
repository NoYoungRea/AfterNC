#pragma once
#include <Windows.h>
#define BUFFER_LENGTH 1000
class BinaryBuffer
{
public:
    int GetUseSize();
    bool PeekData(BYTE* byDest, int size);
    bool GetData(BYTE* byDest, int size);
    bool SystemPutData(int size);
    void MoveRemainingData();
    void MoveStartCursor(int size);

private:
    DWORD m_UseSize;
    BYTE m_Buffer[BUFFER_LENGTH];
    BYTE* m_pEndBuffer;
    BYTE* m_pStartCursor;
    BYTE* m_pEndCursor;
};
