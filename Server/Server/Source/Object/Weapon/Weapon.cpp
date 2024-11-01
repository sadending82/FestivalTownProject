#pragma once
#include "Weapon.h"


Weapon::~Weapon()
{

}

void Weapon::WeaponInit(eWeaponType type, WeaponStat& stat)
{
	mWeaponType = type;
	mStat = stat;
	mOwnerID = INVALIDKEY;
	SetIsGrabbed(false);
	SetUniqueCode();
}
