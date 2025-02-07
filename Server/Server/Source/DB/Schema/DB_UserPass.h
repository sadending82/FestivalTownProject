#pragma once

#pragma pack (push, 1)

enum class UserPass_Field {
	userUID = 1,
	passCode,
	passType,
	passLevel,
	passExp
};

struct UserPass {
	int user_UID;
	int passCode;
	int passType;
	int passLevel;
	int passExp;
};

#pragma pack(pop) 