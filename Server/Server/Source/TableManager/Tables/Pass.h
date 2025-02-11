#pragma once
#include <ctime>

#pragma pack (push, 1)

#define PassList_Sheet 1
#define PassLevel_Sheet 2
#define PassMission_Sheet 3

enum class PassList_Field {
	index = 0,
	name,
	limit_exp_daily_mission,
	level_repeated_reward,
	open_date,
	close_date,
};

enum class PassLevel_Field {
	index = 0,
	pass,
	level,
	Exp_Required,
	Pass_Type,
	Reward_Item_Index,
	Reward_Item_Amount
};

enum class PassMission_Field {
	index = 0,
	pass_index,
	type,
	mission_category,
	mission_group,
	mission_step,
	mission_name,
	mission_description,
	required_item_grade,
	required_count,
	reward_exp,
	reward_item,
	reward_item_amount

};

struct PassLevel {
	int index = 0;
	int pass = 0;
	int level = 0;
	int Exp_Required = 0;
	int Pass_Type = 0;
	int Reward_Item_Index = 0;
	int Reward_Item_Amount = 0;
};

struct PassInfo {
	int index;
	int limit_exp_daily_mission;
	int level_repeated_reward;
	std::tm open_date;
	std::tm close_date;

	// <level, <type, info>>
	std::unordered_map<int, std::unordered_map<int, PassLevel>> passLevelList;
};

struct PassMissionInfo {
    int index;
	int pass_index;
    int type;
    int mission_category;
    int mission_group;

    int mission_step;

	int required_item_grade;
    int required_count;

    int reward_exp;
    int reward_item;
	int reward_item_amount;

    bool isRewarded;
};

#pragma pack(pop) 