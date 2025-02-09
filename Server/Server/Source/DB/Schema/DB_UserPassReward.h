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
	bool isRewarded = 0;
};

#pragma pack(pop) 