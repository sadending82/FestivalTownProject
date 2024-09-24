#pragma once
#include "../Network/Session/Session.h"

class Player : public Session
{
public:
	Player() : mRoomID(INVALIDKEY)
		, mInGameID(INVALIDKEY)
	{}
	~Player() {}

	void			Init();

	virtual void	Disconnect() override;
	
	virtual void	DoRecv() override;;

	virtual void	DoSend(void* packet, const int size) override;

	int				GetUID() { return mUID; }
	std::string		GetAccountID() { return mAccountID; }
	int				GetRoomID() { return mRoomID; }
	int				GetInGameID() { return mInGameID; }
	int				GetTeam() { return mTeam; }
	std::string		GetName() { return mName; }
	Vector3f		GetPosition() { return mPosition; }
	Vector3f		GetDirection() { return mDirection; }
	ePlayerState	GetPlayerState() { return mPlayerState; }
	std::mutex&		GetPlayerStateLock() { return mPlayerStateLock; }
	eCharacterType	GetChacracterType() { return mCharacterType; }
	int				GetHP() { return mHP; }
	int				GetStamina() { return mStamina; }
	int				GetGroggyCount() { return mGroggyCount; }
	class Bomb*		GetBomb() { return mBomb; }
	class Weapon*	GetWeapon() { return mWeapon; }

	int				SetUID() { return mUID; }
	void			SetAccountID(std::string accountID) { mAccountID = accountID; }
	void			SetRoomID(int id) { mRoomID = id; }
	void			SetInGameID(int id) { mInGameID = id; }
	void			SetTeam(int team) { mTeam = team; }
	void			SetName(std::string name) { mName = name; }
	void			SetPosition(float x, float y, float z) { mPosition = Vector3f(x, y, z); }
	void			SetPosition(Vector3f v3f) { mPosition = v3f; }
	void			SetDirection(float x, float y, float z) { mDirection = Vector3f(x, y, z); }
	void			SetDirection(Vector3f v3f) { mDirection = v3f; }
	void			SetPlayerState(ePlayerState state) { mPlayerState = state; }
	void			SetChacracterType(eCharacterType type) { mCharacterType = type; }
	void			SetHP(int hp) { mHP = hp; }
	void			SetStamina(int stamina) { mStamina = stamina; }
	void			SetGroggyCount(int count) { mGroggyCount = count; }
	void			SetBomb(class Bomb* bomb) { mBomb = bomb; }
	void			SetWeapon(class Weapon* weapon) { mWeapon = weapon; }

	void			RecoveryHP(int value) { mHP += value; }
	void			ReduceHP(int value) { mHP -= value; }

	void			AddGroggyCount() { mGroggyCount++; }

	int				GroggyRecoverTime();

	void			ChangeToGroggyState(class Server* pServer, int roomID);
	void			ChangeToDeadState(class Server* pServer, int roomID);

protected:
	std::mutex      mPlayerStateLock;
	ePlayerState	mPlayerState;

	int				mUID;	// GameDB에서 PK로 사용하는 Unique ID
	std::string		mAccountID = ""; // 계정 ID
	int				mRoomID;
	int				mInGameID; // 클라와 함께 인게임 내에서 구분하기 위한 id
	std::string		mName = "test"; // 임시 닉네임
	int				mTeam;
	
	Vector3f		mPosition;
	Vector3f		mDirection;

	eCharacterType	mCharacterType = eCharacterType::CT_TEST;
	int				mHP;
	int				mStamina;
	int				mGroggyCount;
	class Bomb*     mBomb = nullptr;
	class Weapon*	mWeapon = nullptr;
};