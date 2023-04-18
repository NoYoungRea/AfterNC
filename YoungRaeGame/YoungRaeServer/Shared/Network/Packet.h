#include "NetworkHeader.h"
#define PacketSize 1000
class Packet
{
public:
    Packet() {
		m_iBufferSize = PacketSize - sizeof(NetworkHeader);
		m_iDataSize = 0;
		m_headerSize = sizeof(NetworkHeader);
		m_cpPayloadPtr = m_cpBuffer + m_headerSize;
		m_isEncoded = FALSE;
	}

	void Decoding(NetworkHeader* pHeader) {

		DWORD count = 1;
		BYTE* bPointer = (BYTE*)m_cpPayloadPtr - sizeof(BYTE);
		*bPointer = pHeader->byCheckSum;
		BYTE prevP = 0;
		BYTE curP = 0;
		BYTE D = 0;
		BYTE E = 0;
		BYTE RK = pHeader->byRandKey;
		while (count != m_iDataSize + 2)
		{
			curP = (*bPointer) ^ (byFixKey + count + E);
			D = curP ^ (RK + count + prevP);

			E = *bPointer;
			*bPointer = D;
			prevP = curP;
			count++;
			bPointer++;

		}
		pHeader->byCheckSum = *((BYTE*)m_cpPayloadPtr - sizeof(BYTE));

	}

	BYTE GetCheckSum() {
		DWORD checkSum = 0;
		BYTE* byPointer = m_cpPayloadPtr;
		for (size_t i = 0; i < m_iDataSize; i++)
		{
			checkSum += *byPointer;
			byPointer++;
		}
		return checkSum % 256;

	}

	void Encoding() {
		if (m_isEncoded == TRUE) {
			if (((NetworkHeader*)m_cpBuffer)->byCode != byCode) {
				throw 1;
			}

			return;


		}

		//header setting
		((NetworkHeader*)m_cpBuffer)->wPayloadSize = m_iDataSize;
		((NetworkHeader*)m_cpBuffer)->byCode = byCode;
		((NetworkHeader*)m_cpBuffer)->byCheckSum = GetCheckSum();
		((NetworkHeader*)m_cpBuffer)->byRandKey = rand() % 255 + 1;

		//encoding
		DWORD count = 1;
		BYTE* bPointer = (BYTE*)m_cpPayloadPtr - sizeof(BYTE);
		BYTE P = 0;
		BYTE E = 0;
		BYTE RK = ((NetworkHeader*)m_cpBuffer)->byRandKey;
		while (count != m_iDataSize + 2)
		{
			P = (*bPointer) ^ (P + RK + count);
			E = P ^ (E + byFixKey + count);
			*bPointer = E;
			count++;
			bPointer++;

		}
		m_isEncoded = TRUE;

	}

	BYTE* GetPayloadPtr() {
		return m_cpPayloadPtr;
	}

	//int flag;
	DWORD m_iBufferSize;
	DWORD m_iDataSize;
	BYTE m_cpBuffer[PacketSize+1];
	BYTE* m_cpPayloadPtr;
	BYTE m_headerSize;
	BOOL m_isEncoded;


};