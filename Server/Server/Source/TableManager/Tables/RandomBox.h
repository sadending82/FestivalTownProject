#pragma once
#include <string>
#pragma pack (push, 1)

enum class RandomBox_Field {
	index			= 0,
	name			= 1,
	Gacha_Group		= 2,
	Pay_Item_Index1	= 3,
	Pay_Item_Value1	= 4,
	Pay_Item_Index2 = 5,
	Pay_Item_Value2 = 6,
	Open_Date		= 7,
	Close_Date		= 8,
};


struct RandomBox {
	int			index;
	std::string	name;
	int			Gacha_Group;
	int			Pay_Item_Index1;
	int			Pay_Item_Value1;
	int			Pay_Item_Index2;
	int			Pay_Item_Value2;
	std::tm		Open_Date;
	std::tm		Close_Date;
	int			Exp;
};


#pragma pack(pop) 