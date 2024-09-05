#pragma once
#include <string>

#pragma pack (push, 1)
struct WeaponStat {
	int				index;
	std::string		name;
	int				Weapon_Power;
	int				Weapon_Limit;
	int				Weapon_Range;
	int				Weapon_Type;
	int				Weapon_StaminaConsume;
};

#pragma pack(pop) 