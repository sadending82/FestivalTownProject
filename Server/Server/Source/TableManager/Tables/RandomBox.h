#pragma once
#include <string>
#pragma pack (push, 1)

enum class RandomBox_Field {
	index			= 0,
	name			= 1,
	Gacha_Group		= 2,
	Pay_Item_Index	= 3,
	Pay_Item_Value	= 4,
	Open_Date		= 5,
	Close_Date		= 6,
	Exp				= 7
};


struct RandomBox {
	int			index;
	std::string	name;
	int			Gacha_Group;
	int			Pay_Item_Index;
	int			Pay_Item_Value;
	int			Open_Date;
	int			Close_Date;
	int			Exp;
};


#pragma pack(pop) 