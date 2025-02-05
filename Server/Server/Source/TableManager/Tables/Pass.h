#pragma once
#include <ctime>

#pragma pack (push, 1)

#define PassList_Sheet 1
#define PassMission_Sheet 2

enum class PassLevel_Field {
	level = 2,
	Exp_Required = 3,
	Pass_Type = 4,
	Reward_Item_Index = 5,
	Reward_Item_Amount = 6,
	Open_Date = 7,
	Close_Date = 8
};

enum class PassMission_Field {
	index = 0,
	type,
	mission_category,
	mission_group,
	mission_step,
	mission_name,
	mission_description,
	required_count,
	reward_exp,
	reward_item,
	reward_item_amount

};

struct PassLevel {
	int level = 0;
	int Exp_Required = 0;
	int Pass_Type = 0;
	int Reward_Item_Index = 0;
	int Reward_Item_Amount = 0;
	std::tm Open_Date;
	std::tm Close_Date;
};

struct PassList {
	// <level, <type, info>>
	std::unordered_map<int, std::unordered_map<int, PassLevel>> passLevelList;
};

struct PassMissionInfo {
    int index;

    int type;
    int mission_category;
    int mission_group;

    int mission_step;

    int required_count;

    int reward_exp;
    int reward_item;
	int reward_item_amount;

    bool isRewarded;
};

#pragma pack(pop) 