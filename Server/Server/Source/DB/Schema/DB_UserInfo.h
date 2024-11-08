#pragma once
#include <string>
#pragma pack (push, 1)

enum class UserInfo_Field {
	UID = 1,
	AccountID,
	NickName,
	UserLevel,
	PassLevel,
	UserTitle,
	ProfileSkin,
	Point,
	LastLoginTime,
	AttendanceDay,
	// characterCustomizing
	State = 12
};


struct UserInfo {
	int UID;
	std::string AccountID = std::string(50, '\0');
	std::string NickName = std::string(20, '\0');
	int UserLevel;
	int PassLevel;
	int UserTitle;
	int ProfileSkin;
	int Point;
	int Gold;
	std::tm date;
	int AttendanceDay;
	// characterCustomizing
	int State;
};


#pragma pack(pop) 