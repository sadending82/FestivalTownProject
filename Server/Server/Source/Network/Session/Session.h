#pragma once
#include "../ExOver.h"

class Session
{
public:
	Session() : mState(eSessionState::ST_FREE)
		, mSessionID(INVALIDKEY)
		,mPrevData(0)
	{
	}
	~Session() {}

	virtual void Disconnect();

	virtual void DoRecv();

	virtual void DoSend(void* packet);

	ExOver GetExOver() { return mExOver; }
	void SetExOver(ExOver over) { mExOver = over; }

	eSessionState GetState() { return mState; }
	void SetState(eSessionState state) { mState = state; }

	std::mutex& GetStateLock() { return mStateLock; }

	int GetSessionID() { return mSessionID; }
	void SetSessionID(int id) { mSessionID = id; }

	SOCKET GetSocket() { return mSocket; }
	void SetSocket(SOCKET sock) { mSocket = sock; }

	int GetPrevData() { return mPrevData; }
	void SetPrevData(int prevData) { mPrevData = prevData; }

private:
	ExOver mExOver;
	std::mutex	mStateLock;
	eSessionState mState;
	int mSessionID;
	SOCKET mSocket;
	int	mPrevData;
};

