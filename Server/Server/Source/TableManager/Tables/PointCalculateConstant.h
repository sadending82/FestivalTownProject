#pragma once
#pragma pack (push, 1)

#define Mode_Point_Sheet 3

enum class PointConstant_Field {
	Mode_Index = 2,	
	Battle_Result = 3,	
	Type = 4,	
	WorkType = 5,	
	Value = 6,
};

enum class ConstantType {
	Kill_Point = 1,
	Death_Point = 2,
	Bomb_Point = 3
};

enum class WorkType {
	Add = 1,
	Sub = 2,
	Mul = 3,
	Div = 4,
};

struct PointConstant {
	int WorkType;
	int Value;
};

struct PointConstants {
	struct PointConstant Win_Kill_Point;
	struct PointConstant Win_Death_Point;
	struct PointConstant Win_Bomb_Point;

	struct PointConstant Lose_Kill_Point;
	struct PointConstant Lose_Death_Point;
	struct PointConstant Lose_Bomb_Point;
};

#pragma pack(pop) 