#include "Player.h"
#include "../Server/Server.h"

void Player::Init()
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
}

void Player::Disconnect()
{
	Init();
	__super::Disconnect();
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

	// µé°íÀÖ´Â ¹«±â ÇØÁ¦
	if (mWeapon != nullptr) {
		if (mWeapon->SetIsGrabbed(false) == true) {
			int weaponID = mWeapon->GetID();
			mWeapon->SetOwenrID(INVALIDKEY);
			mWeapon->SetPosition(mPosition);
			mWeapon = nullptr;
			pServer->GetPacketSender()->SendWeaponDropPacket(mPosition, mRoomID, weaponID);
		}
	}

	// µé°íÀÖ´Â ÆøÅº Æø¹ß
	if (mBomb != nullptr) {
		if (mBomb->SetIsGrabbed(false) == true) {
			int bombID = mBomb->GetID();
			mBomb = nullptr;
			pServer->GetPacketSender()->SendBombExplosionPacket(mRoomID, bombID);
			pServer->GetRooms()[mRoomID]->DeleteBomb(bombID);
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

	mPlayerState = ePlayerState::PS_DEAD;

	pServer->GetPacketSender()->SendPlayerDeadPacket(mInGameID, mRoomID, spawn_time);

	// µé°íÀÖ´Â ¹«±â ÇØÁ¦
	if (mWeapon != nullptr) {
		if (mWeapon->SetIsGrabbed(false) == true) {
			int weaponID = mWeapon->GetID();
			mWeapon->SetOwenrID(INVALIDKEY);
			mWeapon->SetPosition(mPosition);
			mWeapon = nullptr;
			pServer->GetPacketSender()->SendWeaponDropPacket(mPosition, mRoomID, weaponID);
		}
	}

	// µé°íÀÖ´Â ÆøÅº Æø¹ß
	if (mBomb != nullptr) {
		if (mBomb->SetIsGrabbed(false) == true) {
			int bombID = mBomb->GetID();
			mBomb = nullptr;
			pServer->GetPacketSender()->SendBombExplosionPacket(mRoomID, bombID);
			pServer->GetRooms()[mRoomID]->DeleteBomb(bombID);
		}
	}

	mPlayerStateLock.unlock();
	return true;
}
