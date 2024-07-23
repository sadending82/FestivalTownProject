#pragma once
#include "../Network/Session/Session.h"
class Player : public Session
{
public:
	Player() {}
	~Player() {}

	
	int GetRoomID() { return mRoomID; }
	void SetRoomID(int id) { mRoomID = id; }

	int GetInGameID() { return mInGameID; }
	void SetInGameID(int id) { mInGameID = id; }

private:
	std::mutex      mPlayerStateLock;
	ePlayerState	mPlayerState;
	int				mRoomID;
	int				mInGameID;
};