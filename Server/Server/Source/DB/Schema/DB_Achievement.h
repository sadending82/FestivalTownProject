#pragma once
#include <ctime>
#pragma pack (push, 1)

struct Achievement {
	int user_UID = 0;
	int achievement_code = 0;
	std::tm	completed_date;
	int progress;
	int required_count = 0;
	int	is_rewarded = 0;
};

#pragma pack(pop) 
