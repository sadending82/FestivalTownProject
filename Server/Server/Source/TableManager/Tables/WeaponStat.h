#pragma once
#include <string>

#pragma pack (push, 1)

enum class WeaponStat_Field {
	index,
	name,
	Weapon_Power,
	Weapon_Range,
	Weapon_Speed,
	Weapon_Type,
	Weapon_StaminaConsume,
};

struct WeaponStat {
	int				index;
	std::string		name;
	int				Weapon_Power;
	float			Weapon_Range;
	float			Weapon_Speed;
	int				Weapon_Type;
	int				Weapon_StaminaConsume;
};

#pragma pack(pop) 