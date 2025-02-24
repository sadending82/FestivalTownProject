#pragma once
#include "../Network/Session/Session.h"
#include "../TableManager/Tables/CharacterStat.h"
#include "../DB/Schema/DB_UserInfo.h"
#include <shared_mutex>

class Player : public Session
{
public:
	Player() : mRoomID(INVALIDKEY)
		, mInGameID(INVALIDKEY)
	{
	}
	~Player() {}

	void				Init();
	void				IngameInfoInit();

	virtual void		Disconnect() override;

	virtual void		DoRecv() override;

	virtual void		DoSend(void* packet, const int size) override;

	int					GetUID() { return mUserInfo.UID; }
	std::string			GetAccountID() { return mUserInfo.AccountID; }
	bool				GetIsAuthenticated() { return mIsAuthenticated; }
	int					GetRoomID() { return mRoomID; }
	int					GetInGameID() { return mInGameID; }
	int					GetTeam() { return mTeam; }
	bool				GetIsBot() { return mIsBot; }
	std::wstring		GetNickName() { return mUserInfo.NickName; }
	Vector3f& GetPosition() { return mPosition; }
	Vector3f& GetDirection() { return mDirection; }
	ePlayerState		GetPlayerState() { return (ePlayerState)mPlayerState.load(); }
	eCharacterType		GetCharacterType() { return mCharacterType; }
	sCharacterCustomizing& GetCharacterCustomizing() { return mUserInfo.characterCustomizing; }
	int					GetHP() { return mHP; }
	int					GetStamina() { return mStamina; }
	int					GetGroggyCount() { return mGroggyCount; }
	class Bomb* GetBomb() { return mBomb; }
	class Weapon* GetWeapon() { return mWeapon; }
	std::mutex& GetBombLock() { return mBombLock; }
	std::shared_mutex& GetWeaponLock() { return mWeaponLock; }
	bool				GetIsGrabbed() { return mIsGrabbed.load(); }
	int					GetAttachedPlayerID() { return mAttachedPlayerID; }
	CharacterStat& GetCharacterStat() { return mCharacterStat; }
	long long			GetLastGroggyTime() { return mLastGroggyTime.load(); }
	std::tm				GetLastLoginTime() { return mUserInfo.LastLogin; }

	UserMissionList&	GetMissionList() { return mMissionList; }

	std::unordered_map<int, PlayerPassInfo>&	GetPassInfo() { return mPassInfoList; }
	std::unordered_map<int, UserItem>& GetItems() { return mItems; }


	void				SetUID(int uid) { mUserInfo.UID = uid; }
	void				SetAccountID(std::string accountID) { mUserInfo.AccountID = accountID; }
	void				SetIsAuthenticated(bool value) { mIsAuthenticated = value; }
	void				SetroomID(int id) { mRoomID = id; }
	void				SetInGameID(int id) { mInGameID = id; }
	void				SetTeam(int team) { mTeam = team; }
	void				SetIsBot(bool isBot) { mIsBot = isBot; }
	void				SetNickName(std::wstring name) { mUserInfo.NickName = name; }
	void				SetPosition(float x, float y, float z) { mPosition = Vector3f(x, y, z); }
	void				SetPosition(Vector3f v3f) { mPosition = v3f; }
	void				SetDirection(float x, float y, float z) { mDirection = Vector3f(x, y, z); }
	void				SetDirection(Vector3f v3f) { mDirection = v3f; }
	void				SetPlayerState(ePlayerState state) { mPlayerState.store((int)state); }
	void				SetChacracterType(eCharacterType type) { mCharacterType = type; }
	void				SetCharacterCustomizing(sCharacterCustomizing& characterCustomizing) { mUserInfo.characterCustomizing = characterCustomizing; }
	void				SetHP(int hp) { mHP.store(hp); }
	void				SetStamina(int stamina) { mStamina.store(stamina); }
	void				SetGroggyCount(int count) { mGroggyCount = count; }
	void				SetBomb(class Bomb* bomb) { mBomb = bomb; }
	void				SetWeapon(class Weapon* weapon) { mWeapon = weapon; }
	void				SetAttachedPlayerID(int playerID) { mAttachedPlayerID = playerID; }
	void				SetCharacterStat(CharacterStat& stat) { mCharacterStat = stat; }
	void				SetLastGroggyTime(long long lastGroggyTime) { mLastGroggyTime.store(lastGroggyTime); }
	void				SetLastLoginTime(std::tm time) { mUserInfo.LastLogin = time; }

	void				SetUserInfoFromDB(UserInfo& userInfo) { mUserInfo = userInfo; }
	// cas
	bool				SetIsGrabbed(bool desired);
	void				SetItems(std::unordered_map<int, UserItem>& items) { mItems = items; }

	void				RecoveryHP(int value) { mHP.fetch_add(value); }
	void				ReduceHP(int value) { mHP.fetch_add(-value); }

	void				ReduceStamina(int value) { mStamina.fetch_add(-value); }

	void				AddGroggyCount() { mGroggyCount++; }

	int					GroggyRecoverTime();

	bool				ChangePlayerState(ePlayerState expected, ePlayerState newState);

	bool				ChangeToGroggyState(class Server* pServer);
	bool				ChangeToDeadState(class Server* pServer, int spawn_time);

	void				ChangeCharacterType(class Server* pServer, eCharacterType type);

protected:
	std::mutex			mBombLock;
	std::shared_mutex	mWeaponLock;

	std::atomic<int>	mPlayerState;

	UserInfo			mUserInfo = UserInfo();

	bool				mIsBot = false;

	bool				mIsAuthenticated = false;

	// InGame Info
	int					mRoomID;
	int					mInGameID; // 클라와 함께 인게임 내에서 구분하기 위한 id
	int					mTeam;

	Vector3f			mPosition;
	Vector3f			mDirection;

	eCharacterType		mCharacterType = eCharacterType::CT_TEST;
	std::atomic<int>	mHP;
	std::atomic<int>	mStamina;
	int					mGroggyCount;
	class Bomb*			mBomb = nullptr;
	class Weapon*		mWeapon = nullptr;
	std::atomic<bool>	mIsGrabbed = false;
	int					mAttachedPlayerID = INVALIDKEY;

	std::atomic<long long>	mLastGroggyTime = 0;

	CharacterStat		mCharacterStat;

	std::unordered_map<int, Achievement> mInProgressAchievements;

	UserMissionList mMissionList;

	// <pass_index, PlayerPassInfo>
	std::unordered_map<int, PlayerPassInfo>	mPassInfoList;

	std::unordered_map<int, UserItem> mItems;
};