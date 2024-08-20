#include "Player.h"

void Player::Init()
{
	mPlayerState = ePlayerState::PS_IDLE;

	mRoomID = INVALIDKEY;
	mInGameID = INVALIDKEY;
	mTeam = INVALIDKEY;

	mPosition = Vector3f(0, 0, 0);
	mDirection = Vector3f(0, 0, 0);
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
