#pragma once
#include <WinSock2.h>
#include <mswsock.h>
#undef max
#undef min

#include "utility.h"

enum class eOpType
{
	OP_ACCEPT, OP_RECV, OP_SEND, OP_EVENT
};

class OverlappedEx
{
public:
	OverlappedEx()
	{
		wsabuf.len = BUFSIZE;
		wsabuf.buf = reinterpret_cast<char*>(iocpBuf);
		eventType = eOpType::OP_RECV;
		ZeroMemory(&over, sizeof(over));
	}

	OverlappedEx(unsigned char* packet, const int size)
	{
		wsabuf.len = size;
		wsabuf.buf = reinterpret_cast<char*>(iocpBuf);
		ZeroMemory(&over, sizeof(over));
		eventType = eOpType::OP_SEND;
		memcpy(iocpBuf, packet, size);
	}

	~OverlappedEx() {}

	WSAOVERLAPPED over;
	WSABUF wsabuf;
	unsigned char iocpBuf[BUFSIZE];
	eOpType eventType;
	int eventTarget;
};

