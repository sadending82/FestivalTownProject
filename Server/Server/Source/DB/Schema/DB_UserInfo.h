#pragma once
#include "DB_UserCustomizing.h"
#include "../../utility.h"
#include <string>
#include <unordered_map>
#pragma pack (push, 1)

enum class UserInfo_Field {
	UID = 1,
	AccountID,
	NickName,
	UserLevel,
	PassLevel,
	Point,
	LastLoginTime,
	AttendanceDay,
	State = 9
};

struct UserInfo {
	int UID = 0;
	std::string AccountID = std::string(50, '\0');
	std::wstring NickName = L"Default";
	int UserLevel = 0;
	int PassLevel = 0;
	int Point = 0;
	std::tm LastLogin;
	int AttendanceDay = 0;
	int State;

	sCharacterCustomizing characterCustomizing;
};


#pragma pack(pop) 