#pragma once
#pragma pack (push, 1)

#define Mode_Point_Sheet 2

enum PointConstant_Field {
	PC_Mode_Index = 2,	
	PC_Battle_Result = 3,	
	PC_Type = 4,	
	PC_WorkType = 5,	
	PC_Value = 6,
};

enum ConstantType {
	CT_Kill_Point = 1,
	CT_Death_Point = 2,
	CT_Bomb_Point = 3
};

enum WorkType {
	WT_Add = 1,
	WT_Sub = 2,
	WT_Mul = 3,
	WT_Div = 4,
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