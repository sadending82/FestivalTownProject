#pragma once
#include "../Network/Session/Session.h"
#include "../TableManager/Tables/CharacterStat.h"
#include "../DB/Tables/DB_UserInfo.h"
#include <shared_mutex>

class Player : public Session
{
public:
	Player() : mRoomID(INVALIDKEY)
		, mInGameID(INVALIDKEY)
	{}
	~Player() {}

	void				Init();

	virtual void		Disconnect() override;
	
	virtual void		DoRecv() override;;

	virtual void		DoSend(void* packet, const int size) override;

	int					GetUID() { return mUID; }
	std::string			GetAccountID() { return mAccountID; }
	int					GetRoomID() { return mRoomID; }
	int					GetInGameID() { return mInGameID; }
	int					GetTeam() { return mTeam; }
	bool				GetIsBot(){ return mIsBot; }
	std::string			GetName() { return mName; }
	Vector3f&			GetPosition() { return mPosition; }
	Vector3f&			GetDirection() { return mDirection; }
	ePlayerState		GetPlayerState() { return mPlayerState; }
	std::shared_mutex&	GetPlayerStateLock() { return mPlayerStateLock; }
	eCharacterType		GetCharacterType() { return mCharacterType; }
	int					GetHP() { return mHP; }
	int					GetStamina() { return mStamina; }
	int					GetGroggyCount() { return mGroggyCount; }
	class Bomb*			GetBomb() { return mBomb; }
	class Weapon*		GetWeapon() { return mWeapon; }
	std::mutex&			GetBombLock() { return mBombLock; }
	std::shared_mutex&	GetWeaponLock() { return mWeaponLock; }
	bool				GetIsGrabbed() { return mIsGrabbed.load(); }
	int					GetAttachedPlayerID() { return mAttachedPlayerID; }
	CharacterStat&		GetCharacterStat() { return mCharacterStat; }

	void				SetUID(int uid) { mUID = uid; }
	void				SetAccountID(std::string accountID) { mAccountID = accountID; }
	void				SetRoomID(int id) { mRoomID = id; }
	void				SetInGameID(int id) { mInGameID = id; }
	void				SetTeam(int team) { mTeam = team; }
	void				SetIsBot(bool isBot) { mIsBot = isBot; }
	void				SetName(std::string name) { mName = name; }
	void				SetPosition(float x, float y, float z) { mPosition = Vector3f(x, y, z); }
	void				SetPosition(Vector3f v3f) { mPosition = v3f; }
	void				SetDirection(float x, float y, float z) { mDirection = Vector3f(x, y, z); }
	void				SetDirection(Vector3f v3f) { mDirection = v3f; }
	void				SetPlayerState(ePlayerState state) { mPlayerState = state; }
	void				SetChacracterType(eCharacterType type) { mCharacterType = type; }
	void				SetHP(int hp) { mHP = hp; }
	void				SetStamina(int stamina) { mStamina = stamina; }
	void				SetGroggyCount(int count) { mGroggyCount = count; }
	void				SetBomb(class Bomb* bomb) { mBomb = bomb; }
	void				SetWeapon(class Weapon* weapon) { mWeapon = weapon; }
	void				SetAttachedPlayerID(int playerID) { mAttachedPlayerID = playerID; }
	void				SetCharacterStat(CharacterStat& stat) { mCharacterStat = stat; }

	void				SetUserInfoFromDB(UserInfo& userInfo);
	// cas
	bool				SetIsGrabbed(bool desired);

	void				RecoveryHP(int value) { mHP += value; }
	void				ReduceHP(int value) { mHP -= value; }

	void				ReduceStamina(int value) { mStamina -= value; }

	void				AddGroggyCount() { mGroggyCount++; }

	int					GroggyRecoverTime();

	bool				ChangeToGroggyState(class Server* pServer);
	bool				ChangeToDeadState(class Server* pServer, int spawn_time);

	void				ChangeCharacterType(class Server* pServer, eCharacterType type);

protected:
	std::shared_mutex	mPlayerStateLock;
	std::mutex			mBombLock;
	std::shared_mutex	mWeaponLock;

	ePlayerState		mPlayerState;

	int					mUID = 0;	// GameDB에서 PK로 사용하는 Unique ID
	std::string			mAccountID = ""; // 계정 ID
	std::string			mName = "test"; // 임시 닉네임

	bool				mIsBot = false;

	int					mRoomID;
	int					mInGameID; // 클라와 함께 인게임 내에서 구분하기 위한 id
	int					mTeam;

	Vector3f			mPosition;
	Vector3f			mDirection;

	eCharacterType		mCharacterType = eCharacterType::CT_TEST;
	int					mHP;
	int					mStamina;
	int					mGroggyCount;
	class Bomb*			mBomb = nullptr;
	class Weapon*		mWeapon = nullptr;
	std::atomic<bool>	mIsGrabbed = false;
	int					mAttachedPlayerID = INVALIDKEY;

	CharacterStat		mCharacterStat;
};