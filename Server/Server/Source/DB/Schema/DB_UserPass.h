#pragma once

#pragma pack (push, 1)

enum class UserPass_Field {
	userUID = 1,
	passIndex,
	passType,
	passLevel,
	passExp,
	daily_mission_exp
};

struct UserPass {
	int user_UID = 0;
	int passIndex = 0;
	int passType = 0;
	int passLevel = 0;
	int passExp = 0;
	int daily_mission_exp = 0;
};

#pragma pack(pop) 