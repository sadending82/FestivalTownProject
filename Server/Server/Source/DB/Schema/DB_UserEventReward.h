#pragma once
#include <ctime>
#pragma pack (push, 1)

enum class UserEventReward_Field {
	userUID = 1,
	eventCode,
	isRewarded,
	rewardedDate
};


struct UserEventReward {
	int userUID = 0;
	int	eventCode = 0;
	bool isRewarded = 0;
	std::tm	rewardedDate = {};
};

#pragma pack(pop) 