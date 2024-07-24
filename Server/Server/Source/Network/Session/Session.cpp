#include "Session.h"

void Session::Disconnect()
{
	mStateLock.lock();
	if (GetState() == eSessionState::ST_FREE) {
		mStateLock.unlock();
		return;
	}
	closesocket(GetSocket());
	SetState(eSessionState::ST_FREE);

	mStateLock.unlock();
	DEBUGMSGONEPARAM("Disconnect: %d\n", GetSessionID());
}

void Session::DoRecv()
{
	DWORD recvFlag = 0;
	ZeroMemory(&mExOver.mOver, sizeof(mExOver.mOver));
	mExOver.mWsaBuf.len = BUFSIZE - mPrevData;
	mExOver.mWsaBuf.buf = reinterpret_cast<char*>(mExOver.mMessageBuf) + mPrevData;
	BOOL ret = WSARecv(mSocket, &mExOver.mWsaBuf, 1, 0, &recvFlag, &mExOver.mOver, 0);
}

void Session::DoSend(void* packet, const int size)
{
	ExOver* sdata = new ExOver{ reinterpret_cast<unsigned char*>(packet), size };

	std:: cout << sdata->mWsaBuf.len << std::endl;
	BOOL ret = WSASend(mSocket, &sdata->mWsaBuf, 1, 0, 0, &sdata->mOver, 0);
}
