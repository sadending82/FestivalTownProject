#include "Player.h"
#include "../Server/Server.h"

void Player::Init()
{
	mUserInfo = UserInfo();

	mIsBot = false;

	mCharacterType = eCharacterType::CT_TEST;

	IngameInfoInit();
}

void Player::IngameInfoInit()
{
	mPlayerState = ePlayerState::PS_NONE;

	mRoomID = INVALIDKEY;
	mInGameID = INVALIDKEY;
	mTeam = INVALIDKEY;

	mPosition = Vector3f(0, 0, 0);
	mDirection = Vector3f(0, 0, 0);

	mBomb = nullptr;
	mWeapon = nullptr;

	mIsGrabbed = false;
	mAttachedPlayerID = INVALIDKEY;
	mCharacterStat = CharacterStat();
}

void Player::Disconnect()
{
	__super::Disconnect();
	Init();
}

void Player::DoRecv()
{
	__super::DoRecv();
}

void Player::DoSend(void* packet, const int size)
{
	__super::DoSend(packet, size);
}

bool Player::SetIsGrabbed(bool desired)
{
	bool expected = !desired;
	return mIsGrabbed.compare_exchange_strong(expected, desired);
}

int Player::GroggyRecoverTime()
{
	if (mGroggyCount > 3 + 1) return 10;

	return mGroggyCount * 2 + 1;
	//return 20;
}

bool Player::ChangeToGroggyState(Server* pServer)
{
	mPlayerStateLock.lock();
	if (mPlayerState == ePlayerState::PS_GROGGY) {
		mPlayerStateLock.unlock();
		return false;
	}

	mPlayerState = ePlayerState::PS_GROGGY;
	mGroggyCount++;

	pServer->GetPacketSender()->SendPlayerGroggyPacket(mInGameID, mRoomID);

	// 들고있는 무기 해제
	mWeaponLock.lock();
	if (mWeapon != nullptr) {
		if (mWeapon->SetIsGrabbed(false) == true) {
			int weaponID = mWeapon->GetID();
			mWeapon->SetOwenrID(INVALIDKEY);
			mWeapon->SetPosition(mPosition);
			mWeapon = nullptr;
			pServer->GetPacketSender()->SendWeaponDropPacket(mPosition, mRoomID, weaponID);
		}
	}
	mWeaponLock.unlock();

	// 들고있는 폭탄 해제
	mBombLock.lock();
	if (mBomb != nullptr) {
		if (mBomb->SetIsGrabbed(false) == true) {
			int bombID = mBomb->GetID();
			mBomb = nullptr;
			pServer->GetPacketSender()->SendBombDropPacket(mPosition, mRoomID, bombID);
		}
	}
	mBombLock.unlock();

	// 잡은 플레이어 놓기
	if (mAttachedPlayerID != INVALIDKEY && GetIsGrabbed() == false) {
		Room* room = pServer->GetRooms().at(mRoomID);
		if (room != nullptr && (room->GetState() == eRoomState::RS_INGAME)) {
			Player* target = dynamic_cast<Player*>(pServer->GetSessions()[room->GetPlayerList()[mAttachedPlayerID]]);

			if (target->SetIsGrabbed(false) == true) {
				int targetID = mAttachedPlayerID;
				mAttachedPlayerID = INVALIDKEY;
				target->SetAttachedPlayerID(INVALIDKEY);
				pServer->GetPacketSender()->SendPlayerThrowOtherPlayerPacket(mRoomID, mInGameID, mPosition, mDirection, targetID, target->GetPosition(), target->GetDirection());
			}
		}
	}

	mPlayerStateLock.unlock();

	return true;
}

bool Player::ChangeToDeadState(Server* pServer, int spawn_time)
{
	mPlayerStateLock.lock();
	if (mPlayerState == ePlayerState::PS_DEAD) {
		mPlayerStateLock.unlock();
		return false;
	}

	mIsGrabbed.store(false);
	mAttachedPlayerID = INVALIDKEY;
	mPlayerState = ePlayerState::PS_DEAD;

	pServer->GetPacketSender()->SendPlayerDeadPacket(mInGameID, mRoomID, spawn_time);

	// 들고있는 무기 해제
	mWeaponLock.lock();
	if (mWeapon != nullptr) {
		if (mWeapon->SetIsGrabbed(false) == true) {
			int weaponID = mWeapon->GetID();
			mWeapon->SetOwenrID(INVALIDKEY);
			mWeapon->SetPosition(mPosition);
			mWeapon = nullptr;
			pServer->GetPacketSender()->SendWeaponDropPacket(mPosition, mRoomID, weaponID);
		}
	}
	mWeaponLock.unlock();

	// 들고있는 폭탄 해제
	mBombLock.lock();
	if (mBomb != nullptr) {
		if (mBomb->SetIsGrabbed(false) == true) {
			int bombID = mBomb->GetID();
			mBomb = nullptr;
			pServer->GetPacketSender()->SendBombDropPacket(mPosition, mRoomID, bombID);
		}
	}
	mBombLock.unlock();

	mPlayerStateLock.unlock();
	return true;
}

void Player::ChangeCharacterType(Server* pServer, eCharacterType type)
{
	mCharacterType = type;
	mCharacterStat = pServer->GetTableManager()->GetCharacterStats()[type];
}
