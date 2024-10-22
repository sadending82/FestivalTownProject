#pragma once
#include "Bomb.h"

Bomb::Bomb()
{

}

Bomb::~Bomb()
{

}

void Bomb::BombInit()
{
	mOwnerID = INVALIDKEY;
	SetIsGrabbed(false);
}
