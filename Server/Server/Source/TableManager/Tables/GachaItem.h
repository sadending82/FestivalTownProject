#pragma once
#include <string>
#pragma pack (push, 1)

enum class GachaItem_Field {
	index				= 0,
	name				= 1,
	Gacha_Group			= 2,
	Gacha_Weight		= 3,
	Reward_Item_Index	= 4,
	Reward_Item_Value	= 5
};


struct GachaItem {
	int			index;
	std::string	name;
	int			Gacha_Group;
	int			Gacha_Weight;
	int			Reward_Item_Index;
	int			Reward_Item_Value;
};


#pragma pack(pop) 