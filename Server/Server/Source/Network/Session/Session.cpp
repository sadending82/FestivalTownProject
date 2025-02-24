#include "Session.h"

void Session::Disconnect()
{
	//mSessionStateLock.lock();
	closesocket(mSocket);
	SetSessionState(eSessionState::ST_FREE);
	//mSessionStateLock.unlock();
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
	SetAuthenticationKey();
}

void Session::SetAuthenticationKey()
{
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	long long millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

	int numDigits = log10(millis) + 1;
	mAuthenticationKey.store((mSessionID + 1) * pow(10, numDigits) + millis);
}

bool Session::ChangeSessionState(eSessionState expected, eSessionState newState)
{
	int expected_int = static_cast<int>(expected);
	return mSessionState.compare_exchange_strong(expected_int, static_cast<int>(newState));
}
