#pragma once
#pragma pack (push, 1)

#define Mode_Reward_Sheet 4
#define Mode_BonusReward_Sheet 5

enum class Reward_Field {
	Mode_Index = 2,
	Battle_Result,
	Reward1_Index,
	Reward1_Value,
	Reward2_Index,
	Reward2_Value,
	Reward3_Index,
	Reward3_Value,
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

	// draw
	int Draw_Reward1_Index;
	int	Draw_Reward1_Value;
	int	Draw_Reward2_Index;
	int	Draw_Reward2_Value;
	int	Draw_Reward3_Index;
	int	Draw_Reward3_Value;
};

enum class BonusReward_Field {
	Mode_Index = 2,
	Battle_Result,
	Type,
	Point,
	Reward1_Index,
	Reward1_Value,
	Reward2_Index,
	Reward2_Value,
	Reward3_Index,
	Reward3_Value,
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

	// draw
	int Draw_Reward1_Index;
	int	Draw_Reward1_Value;
	int	Draw_Reward2_Index;
	int	Draw_Reward2_Value;
	int	Draw_Reward3_Index;
	int	Draw_Reward3_Value;

	// MVP
	int MVP_Reward1_Index;
	int	MVP_Reward1_Value;
	int	MVP_Reward2_Index;
	int	MVP_Reward2_Value;
	int	MVP_Reward3_Index;
	int	MVP_Reward3_Value;
};

#pragma pack(pop) 