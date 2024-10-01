#pragma once
#include <string>

#define Ch_Stat_Sheet 0
#define Ch_Attack_Sheet 1
#define Ch_Move_Sheet 2
#define Ch_Action_Sheet 3

#pragma pack (push, 1)

enum class CharacterStat_Field {
	index,
	name,
	hp,
	stamina,
	strength,
	speed,
};

enum class AttackStat_Field {
	Index,
	Name,
	Value,
	Attack_Speed,
	Ch_StaminaConsume,
	Steal_Stamina,
};

enum class MoveStat_Field {
	Index,
	Name,
	Value,
	Ch_StaminaConsume,
	Ch_Stamina_recovery,
};

struct CharacterStat {
	int				index;
	std::string		name;
	int				hp;
	int				stamina;
	int				strength;
	int				speed;
};

struct AttackStat {
	int Index;
	std::string Name;
	float Value;
	int	Attack_Speed;
	int	Ch_StaminaConsume;
	int Steal_Stamina;

};

struct MoveStat {
	int Index;
	std::string Name;
	float Value;
	int	Ch_StaminaConsume;
	int Ch_Stamina_recovery;
};

#pragma pack(pop) 