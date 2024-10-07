#pragma once
#include "../Object.h"
#include "../../TableManager/Tables/WeaponStat.h"

class Weapon : public Object
{
public:
	Weapon(eWeaponType type, WeaponStat& stat) : mType(type), mStat(stat) {}
	~Weapon();

	eWeaponType GetType() { return mType; }
	WeaponStat&	GetStat() { return mStat; }

private:
	eWeaponType mType;
	WeaponStat  mStat = WeaponStat();
};