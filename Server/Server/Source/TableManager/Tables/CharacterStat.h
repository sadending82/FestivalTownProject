#pragma once
#include <string>
#include <unordered_map>

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
	Attack_Range,
	Ch_StaminaConsume,
	Vanish_Stamina,
};

enum class MoveStat_Field {
	Index,
	Name,
	Value,
	Ch_StaminaConsume,
	Ch_Stamina_recovery,
};

struct AttackStat {
	int Index;
	std::string Name;
	float Value;
	float Attack_Speed;
	float Attack_Range;
	int	Ch_StaminaConsume;
	int Vanish_Stamina;

};

struct MoveStat {
	int Index;
	std::string Name;
	float Value;
	int	Ch_StaminaConsume;
	int Ch_Stamina_recovery;
};

struct CharacterStat {
	int				index;
	std::string		name;
	float			hp;
	int				stamina;
	float			strength;
	int				speed;

	std::unordered_map<int, AttackStat> attackStats;
	std::unordered_map<int, MoveStat> moveStats;
};

#pragma pack(pop) 