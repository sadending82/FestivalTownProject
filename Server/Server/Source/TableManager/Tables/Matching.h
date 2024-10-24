#pragma once
#pragma pack (push, 1)

enum class Matching_Field {
	Matching_Mode,
	Matching_Min_Time,
	Matching_Max_Time,
	Mode1_Index,
	Mode2_Index,
	Recruit_Player,
};

struct Matching{
	int Matching_Mode;
	int	Matching_Min_Time;
	int	Matching_Max_Time;
	int	Mode1_Index;
	int	Mode2_Index;
	int	Recruit_Player;
};

#pragma pack(pop) 