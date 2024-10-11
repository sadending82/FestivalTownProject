#pragma once
#include <string>
#pragma pack (push, 1)

enum class UserInfo_Field {
	UID,
	AccountID,
	NickName,
	Point,
	Gold,
	LastLoginTime,
	AttendanceDay,
	State
};


struct UserInfo {
	int UID;
	std::string AccountID = std::string(50, '\0');
	std::string NickName = std::string(20, '\0');
	int Point;
	int Gold;
	std::tm date;
	int AttendanceDay;
	int State;
};


#pragma pack(pop) 