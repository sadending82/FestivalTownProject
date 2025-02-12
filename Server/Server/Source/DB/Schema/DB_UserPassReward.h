#pragma once
#include <ctime>

#pragma pack (push, 1)

enum class UserPassReward_Field {
	userUID = 1,
	passIndex,
	passType,
	level,
	isRewarded,
};


struct UserPassReward {
	int userUID = 0;
	int passIndex;
	int passType = 0;
	int level = 0;
	int isRewarded = 0;
};

struct ReceivedPassReward {
	int result;

	int pass_index;
	int pass_type;
	int pass_level;

	int reward_item_index;
	int reward_item_amount;
};

#pragma pack(pop) 