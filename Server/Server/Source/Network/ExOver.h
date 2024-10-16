#pragma once

#include <winsock2.h>
#include <mswsock.h>
// 이거 안쓰면 Flatbuffers 관련 헤더랑 충돌 일어남
#undef max
#undef min

#include <mutex>
#include "../utility.h"
#include "../protocol.h"

enum class eOpType
{
	OP_ACCEPT, OP_RECV, OP_SEND, OP_EVENT
};

class ExOver {
public:
	ExOver()
	{
		mWsaBuf.len = BUFSIZE;
		mWsaBuf.buf = reinterpret_cast<char*>(mMessageBuf);
		mOpType = eOpType::OP_RECV;
		ZeroMemory(&mOver, sizeof(mOver));
	}
	ExOver(unsigned char* packet, const int size)
	{

		mWsaBuf.len = size;
		mWsaBuf.buf = reinterpret_cast<char*>(mMessageBuf);
		ZeroMemory(&mOver, sizeof(mOver));
		mOpType = eOpType::OP_SEND;
		memcpy(mMessageBuf, packet, size);
	}

	~ExOver() {}

	void SetOver(WSAOVERLAPPED over) { mOver = over; }
	void SetWsaBuf(WSABUF buf) { mWsaBuf = buf; }
	void SetMessgeBuf(char* buf) { memcpy(mMessageBuf, buf, sizeof(buf)); }
	void SetOpType(eOpType type) { mOpType = type; }
	void SetTargetID(int id) { mTargetID = id; }

	WSAOVERLAPPED GetOver() { return mOver; }
	WSABUF GetWsaBuf() { return mWsaBuf; }
	unsigned char* GetMessageBuf() { return mMessageBuf; }
	eOpType GetOpType() { return mOpType; }
	int GetTargetID() { return mTargetID; }

	//private:
	WSAOVERLAPPED mOver;
	WSABUF mWsaBuf;
	unsigned char mMessageBuf[BUFSIZE];
	eOpType mOpType;
	int mTargetID;
};
