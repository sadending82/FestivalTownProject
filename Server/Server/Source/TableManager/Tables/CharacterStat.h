#pragma once
#include <string>

#pragma pack (push, 1)
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