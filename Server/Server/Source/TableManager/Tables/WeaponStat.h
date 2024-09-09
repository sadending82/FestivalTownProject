#pragma once
#include <string>

#pragma pack (push, 1)

enum class WeaponStat_Field {
	index,
	name,
	Weapon_Power,
	Weapon_Limit,
	Weapon_Range,
	Weapon_Type,
	Weapon_StaminaConsume,
};

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