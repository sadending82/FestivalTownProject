#pragma once
#include "../Network/Session/Session.h"

class Player : public Session
{
public:
	Player() : mRoomID(INVALIDKEY)
		, mInGameID(INVALIDKEY)
	{}
	~Player() {}

	void Init();

	virtual void Disconnect() override;
	
	virtual void DoRecv() override;;

	virtual void DoSend(void* packet, const int size) override;

	int GetRoomID() { return mRoomID; }
	int GetInGameID() { return mInGameID; }
	int GetTeam() { return mTeam; }
	Vector3f GetPosition() { return mPosition; }
	Vector3f GetDirection() { return mDirection; }
	ePlayerState GetPlayerState() { return mPlayerState; }
	std::mutex& GetPlayerStateLock() { return mPlayerStateLock; }

	void SetRoomID(int id) { mRoomID = id; }
	void SetInGameID(int id) { mInGameID = id; }
	void SetTeam(int team) { mTeam = team; }
	void SetPosition(float x, float y, float z) { mPosition = Vector3f(x, y, z); }
	void SetPosition(Vector3f v3f) { mPosition = v3f; }
	void SetDirection(float x, float y, float z) { mDirection = Vector3f(x, y, z); }
	void SetDirection(Vector3f v3f) { mDirection = v3f; }
	void SetPlayerState(ePlayerState state) { mPlayerState = state; }

protected:
	std::mutex      mPlayerStateLock;
	ePlayerState	mPlayerState;

	int				mRoomID;
	int				mInGameID;
	int				mTeam;
	
	Vector3f		mPosition;
	Vector3f		mDirection;
};