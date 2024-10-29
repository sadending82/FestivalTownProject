#pragma once
#include "../ExOver.h"

class Session
{
public:
	Session() : mSocket(INVALID_SOCKET),
		mSessionState(eSessionState::ST_FREE)
		, mSessionID(INVALIDKEY)
		,mPrevDataSize(0)
		,mIsHeartbeatAck(false)
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
	int GetPrevDataSize() { return mPrevDataSize; }
	bool GetIsHeartbeatAck() { return mIsHeartbeatAck; }

	eMatchingType GetMatchingRequestType() { return mMatchingRequestType; }
	long long GetMatchingRequestTime() { return mMatchingRequestTime.load(); }

	void SetExOver(ExOver over) { mExOver = over; }
	void SetSessionState(eSessionState state) { mSessionState = state; }
	void SetSessionID(int id) { mSessionID = id; }
	void SetSocket(SOCKET sock) { mSocket = sock; }
	void SetPrevDataSize(int prevDataSize) { mPrevDataSize = prevDataSize; }
	void SetIsHeartbeatAck(bool flag) { mIsHeartbeatAck = flag; }

	void SetMatchingRequestType(eMatchingType type) { mMatchingRequestType = type; }
	void SetMatchingRequestTime(long long time) { mMatchingRequestTime.store(time); }

protected:
	ExOver						mExOver;
	std::mutex					mSessionStateLock;
	std::mutex					mDisconnectLock;
	eSessionState				mSessionState;
	int							mSessionID; // 서버 내에서 구분하기 위한 id
	SOCKET						mSocket;
	int							mPrevDataSize;
	bool						mIsHeartbeatAck;

	eMatchingType				mMatchingRequestType;
	std::atomic<long long>		mMatchingRequestTime;
};

