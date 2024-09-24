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

int Player::GroggyRecoverTime()
{
	if (mGroggyCount > 3 + 1) return 10;

	return mGroggyCount * 2 + 1;
}

void Player::ChangeToGroggyState(Server* pServer, int roomID)
{
	mPlayerStateLock.lock();
	mPlayerState = ePlayerState::PS_GROGGY;
	mGroggyCount++;

	pServer->GetPacketSender()->SendPlayerGroggyPacket(mInGameID, roomID);

	// 들고있는 무기 해제
	if (mWeapon != nullptr) {
		if (mWeapon->SetIsGrabbed(false) == true) {
			int weaponID = mWeapon->GetID();
			mWeapon->SetOwenrID(INVALIDKEY);
			mWeapon = nullptr;
			mWeapon->SetPosition(mPosition);
			pServer->GetPacketSender()->SendWeaponDropPacket(mPosition, roomID, weaponID);
		}
	}
	mPlayerStateLock.unlock();
}

void Player::ChangeToDeadState(Server* pServer, int roomID)
{
	mPlayerStateLock.lock();
	mPlayerState = ePlayerState::PS_DEAD;

	pServer->GetPacketSender()->SendPlayerDeadPacket(mInGameID, roomID);

	// 들고있는 무기 해제
	if (mWeapon != nullptr) {
		if (mWeapon->SetIsGrabbed(false) == true) {
			int weaponID = mWeapon->GetID();
			mWeapon->SetOwenrID(INVALIDKEY);
			mWeapon->SetPosition(mPosition);
			mWeapon = nullptr;
			pServer->GetPacketSender()->SendWeaponDropPacket(mPosition, roomID, weaponID);
		}
	}
	mPlayerStateLock.unlock();
}
