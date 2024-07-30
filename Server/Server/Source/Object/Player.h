#pragma once
#include "../Network/Session/Session.h"

class Player : public Session
{
public:
	Player() {}
	~Player() {}

	
	int GetRoomID() { return mRoomID; }
	int GetInGameID() { return mInGameID; }
	Vector3f GetPosition() { return mPosition; }
	Vector3f GetDirection() { return mDirection; }

	void SetRoomID(int id) { mRoomID = id; }
	void SetInGameID(int id) { mInGameID = id; }
	void SetPosition(float x, float y, float z) { mPosition = Vector3f(x, y, z); }
	void SetDirection(float x, float y, float z) { mDirection = Vector3f(x, y, z); }

private:
	std::mutex      mPlayerStateLock;
	ePlayerState	mPlayerState;
	int				mRoomID;
	int				mInGameID;
	
	Vector3f		mPosition;
	Vector3f		mDirection;
};