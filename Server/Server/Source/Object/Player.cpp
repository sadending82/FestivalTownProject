#include "Player.h"

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
