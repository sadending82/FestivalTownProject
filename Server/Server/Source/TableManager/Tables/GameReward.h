#pragma once
#pragma pack (push, 1)

#define Mode_Point 2

enum Reward_Field {
	Reward1_Index,
	Reward1_Value,
	Reward2_Index,
	Reward2_Value,
	Reward3_Index,
	Reward3_Value,
};

struct GameReward {
	int Reward1_Index;
	int	Reward1_Value;
	int	Reward2_Index;
	int	Reward2_Value;
	int	Reward3_Index;
	int	Reward3_Value;
};

#pragma pack(pop) 