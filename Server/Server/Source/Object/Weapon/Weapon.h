#pragma once
#include "../Object.h"
#include "../../TableManager/Tables/WeaponStat.h"

class Weapon : public Object
{
public:
	Weapon() {}
	Weapon(int id, eObjectType type) : Object(id, type) {}
	~Weapon();

	void WeaponInit(eWeaponType type, WeaponStat& stat);

	eWeaponType GetWeaponType() { return mWeaponType; }
	WeaponStat&	GetStat() { return mStat; }

private:
	eWeaponType mWeaponType;
	WeaponStat  mStat = WeaponStat();
};