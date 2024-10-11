#pragma once
#include "../ExOver.h"

class Session
{
public:
	Session() : mSocket(INVALID_SOCKET),
		mSessionState(eSessionState::ST_FREE)
		, mSessionID(INVALIDKEY)
		,mPrevData(0)
		,mIsHeartbeatAck(false)
		,mPlayedSoloGameBefore(false)
		,mMatchingRequestTime(0)
	{
	}
	virtual ~Session() {}

	virtual void Disconnect();

	virtual void DoRecv();

	virtual void DoSend(void* packet, const int size);

	void SessionInit(SOCKET sock, int key);

	ExOver GetExOver() { return mExOver; }
	eSessionState GetSessionState() { return mSessionState; }
	std::mutex& GetSessionStateLock() { return mSessionStateLock; }
	std::mutex& GetDisconnectLock() { return mDisconnectLock; }
	int GetSessionID() { return mSessionID; }
	SOCKET GetSocket() { return mSocket; }
	int GetPrevData() { return mPrevData; }
	bool GetIsHeartbeatAck() { return mIsHeartbeatAck; }
	bool GetPlayedSoloGameBefore() { return mPlayedSoloGameBefore; }
	unsigned int GetMatchingRequestTime() { return mMatchingRequestTime; }

	void SetExOver(ExOver over) { mExOver = over; }
	void SetSessionState(eSessionState state) { mSessionState = state; }
	void SetSessionID(int id) { mSessionID = id; }
	void SetSocket(SOCKET sock) { mSocket = sock; }
	void SetPrevData(int prevData) { mPrevData = prevData; }
	void SetIsHeartbeatAck(bool flag) { mIsHeartbeatAck = flag; }
	void SetPlayedSoloGameBefore(bool flag) { mPlayedSoloGameBefore = flag; }
	void SetMatchingRequestTime(int time) { mMatchingRequestTime = time; }

protected:
	ExOver			mExOver;
	std::mutex		mSessionStateLock;
	std::mutex		mDisconnectLock;
	eSessionState	mSessionState;
	int				mSessionID; // 서버 내에서 구분하기 위한 id
	SOCKET			mSocket;
	int				mPrevData;
	bool			mIsHeartbeatAck;

	bool			mPlayedSoloGameBefore;
	unsigned int	mMatchingRequestTime;
};

