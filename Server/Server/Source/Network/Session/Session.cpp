#include "Session.h"

void Session::Disconnect()
{
	mSessionStateLock.lock();
	closesocket(mSocket);
	SetSessionState(eSessionState::ST_FREE);
	mSessionStateLock.unlock();
	DEBUGMSGONEPARAM("Disconnect: %d\n", GetSessionID());
}

void Session::DoRecv()
{
	DWORD recvFlag = 0;
	ZeroMemory(&mExOver.mOver, sizeof(mExOver.mOver));
	mExOver.mWsaBuf.len = BUFSIZE - mPrevDataSize;
	mExOver.mWsaBuf.buf = reinterpret_cast<char*>(mExOver.mMessageBuf) + mPrevDataSize;
	BOOL ret = WSARecv(mSocket, &mExOver.mWsaBuf, 1, 0, &recvFlag, &mExOver.mOver, 0);
}

void Session::DoSend(void* packet, const int size)
{
	ExOver* sdata = new ExOver{ reinterpret_cast<unsigned char*>(packet), size };
	BOOL ret = WSASend(mSocket, &sdata->mWsaBuf, 1, 0, 0, &sdata->mOver, 0);
}

void Session::SessionInit(SOCKET sock, int key)
{
	mSocket = sock;
	mExOver.SetOpType(eOpType::OP_RECV);
	mPrevDataSize = 0 ;
	mSessionID = key;
}
