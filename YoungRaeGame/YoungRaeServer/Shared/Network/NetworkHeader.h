#pragma once
#include <Windows.h>

//header ���⸸ �����ϸ� ��. ũ�� fix ���ѳ��� �� ����.
#pragma pack(push,1)
struct NetworkHeader {
	BYTE byCode;
	WORD wPayloadSize;
	BYTE byRandKey;
	BYTE byCheckSum;
};

struct Req_Echo
{
	WORD type;
	INT64 AccountNo;
	LONGLONG SendTick;
};

struct Ack_Echo
{
	WORD type;
	INT64 AccountNo;
	LONGLONG SendTick;
};

struct Req_Login
{
	WORD type;
	INT64 AccountNo;
	char SessionKey[64];
	int Version;
};

struct Ack_Login
{
	WORD type;
	BYTE status;
	INT64 accountNo;
};

extern BYTE byFixKey;
extern BYTE byCode;
#pragma pack(pop)

