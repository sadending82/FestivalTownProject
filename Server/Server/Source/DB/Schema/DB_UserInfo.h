#pragma once
#include "DB_UserItem.h"
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
	UserTitle,
	ProfileSkin,
	Point,
	LastLoginTime,
	AttendanceDay,
	characterCustomizing,
	State = 12
};

struct sCharacterCustomizing {
	std::unordered_map<CustomizingItemType, EquippedItem> customizingItems;

	void SetItem(CustomizingItemType type, EquippedItem item) {
		customizingItems[type] = item;
	}

	EquippedItem GetItemCode(CustomizingItemType type) {
		return customizingItems[type];
	}
};

struct UserInfo {
	int UID;
	std::string AccountID = std::string(50, '\0');
	std::wstring NickName = std::wstring(50, '\0');
	int UserLevel;
	int PassLevel;
	int UserTitle;
	int ProfileSkin;
	int Point;
	std::tm date;
	int AttendanceDay;
	sCharacterCustomizing characterCustomizing;
	int State;

	int Gold;
	int Dia;
	int Mileage;
};


#pragma pack(pop) 