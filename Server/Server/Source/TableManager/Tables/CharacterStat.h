#pragma once
#include <string>

#pragma pack (push, 1)

enum class CharacterStat_Field {
	index,
	name,
	hp,
	stamina,
	strength,
	speed,
	attack,
	headAttack,
	jumpKick,
	powerAttack,
	walkSpeed,
	runSpeed,
	rollSpeed,
};

struct CharacterStat {
	int				index;
	std::string		name;
	int				hp;
	int				stamina;
	int				strength;
	int				speed;
	int				attack;
	int				headAttack;
	int				jumpKick;
	int				powerAttack;
	int				walkSpeed;
	int				runSpeed;
	int				rollSpeed;
};

#pragma pack(pop) 