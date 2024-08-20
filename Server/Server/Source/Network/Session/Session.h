#pragma once
#include "../ExOver.h"

class Session
{
public:
	Session() : mSocket(INVALID_SOCKET),
		mState(eSessionState::ST_FREE)
		, mSessionID(INVALIDKEY)
		,mPrevData(0)
		,mIsHeartbeatAck(false)
	{
	}
	virtual ~Session() {}

	virtual void Disconnect();

	virtual void DoRecv();

	virtual void DoSend(void* packet, const int size);

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

	bool GetIsHeartbeatAck() { return mIsHeartbeatAck; }
	void SetIsHeartbeatAck(bool flag) { mIsHeartbeatAck = flag; }

protected:
	ExOver mExOver;
	std::mutex	mStateLock;
	eSessionState mState;
	int mSessionID;
	SOCKET mSocket;
	int	mPrevData;
	bool mIsHeartbeatAck;
};

