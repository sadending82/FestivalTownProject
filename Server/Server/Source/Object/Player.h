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
	eCharacterType GetChacracterType() { return mCharacterType; }
	int GetHP() { return mHP; }
	class Bomb* GetBomb() { return mBomb; }
	class Weapon* GetWeapon() { return mWeapon; }

	void SetRoomID(int id) { mRoomID = id; }
	void SetInGameID(int id) { mInGameID = id; }
	void SetTeam(int team) { mTeam = team; }
	void SetPosition(float x, float y, float z) { mPosition = Vector3f(x, y, z); }
	void SetPosition(Vector3f v3f) { mPosition = v3f; }
	void SetDirection(float x, float y, float z) { mDirection = Vector3f(x, y, z); }
	void SetDirection(Vector3f v3f) { mDirection = v3f; }
	void SetPlayerState(ePlayerState state) { mPlayerState = state; }
	void SetChacracterType(eCharacterType type) { mCharacterType = type; }
	void SetHP(int hp) { mHP = hp; }
	void SetBomb(class Bomb* bomb) { mBomb = bomb; }
	void SetWeapon(class Weapon* weapon) { mWeapon = weapon; }

	void RecoverHP(int value) { mHP += value; }
	void ReduceHP(int value) { mHP -= value; }

protected:
	std::mutex      mPlayerStateLock;
	ePlayerState	mPlayerState;

	int				mRoomID;
	int				mInGameID; // 클라와 함께 인게임 내에서 구분하기 위한 id
	int				mTeam;
	
	Vector3f		mPosition;
	Vector3f		mDirection;

	eCharacterType	mCharacterType = eCharacterType::CT_TEST;
	int				mHP;
	class Bomb*     mBomb = nullptr;
	class Weapon*	mWeapon = nullptr;
};