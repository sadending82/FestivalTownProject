#pragma once
#pragma pack (push, 1)

#define Mode_Point_Sheet 2

enum ScoreConstant_Field {
	SC_Mode_Index = 2,	
	SC_Battle_Result = 3,	
	SC_Type = 4,	
	SC_WorkType = 5,	
	SC_Value = 6,
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

struct ScoreConstant {
	int type;
	int WorkType;
	int Value;
};

#pragma pack(pop) 