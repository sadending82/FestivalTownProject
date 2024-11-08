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
	characterCustomizing,
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
	std::tm date;
	int AttendanceDay;
	std::string characterCustomizing = std::string(20, '\0');
	int State;

	int Gold;
	int Dia;
	int Mileage;
};


#pragma pack(pop) 