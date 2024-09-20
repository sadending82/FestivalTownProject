#pragma once
#pragma pack (push, 1)

#define Mode_Reward_Sheet 3
#define Mode_BonusReward_Sheet 4

enum Reward_Field {
	RW_Mode_Index = 2,
	RW_Battle_Result,
	RW_Reward1_Index,
	RW_Reward1_Value,
	RW_Reward2_Index,
	RW_Reward2_Value,
	RW_Reward3_Index,
	RW_Reward3_Value,
};

struct GameReward {
	// win
	int Win_Reward1_Index;
	int	Win_Reward1_Value;
	int	Win_Reward2_Index;
	int	Win_Reward2_Value;
	int	Win_Reward3_Index;
	int	Win_Reward3_Value;

	// lose
	int Lose_Reward1_Index;
	int	Lose_Reward1_Value;
	int	Lose_Reward2_Index;
	int	Lose_Reward2_Value;
	int	Lose_Reward3_Index;
	int	Lose_Reward3_Value;
};

enum BonusReward_Field {
	BR_Mode_Index = 2,
	BR_Battle_Result,
	BR_Type,
	BR_Point,
	BR_Reward1_Index,
	BR_Reward1_Value,
	BR_Reward2_Index,
	BR_Reward2_Value,
	BR_Reward3_Index,
	BR_Reward3_Value,
};

struct BonusReward {
	// win
	int Win_Reward1_Index;
	int	Win_Reward1_Value;
	int	Win_Reward2_Index;
	int	Win_Reward2_Value;
	int	Win_Reward3_Index;
	int	Win_Reward3_Value;

	// lose
	int Lose_Reward1_Index;
	int	Lose_Reward1_Value;
	int	Lose_Reward2_Index;
	int	Lose_Reward2_Value;
	int	Lose_Reward3_Index;
	int	Lose_Reward3_Value;

	// MVP
	int MVP_Reward1_Index;
	int	MVP_Reward1_Value;
	int	MVP_Reward2_Index;
	int	MVP_Reward2_Value;
	int	MVP_Reward3_Index;
	int	MVP_Reward3_Value;
};

#pragma pack(pop) 