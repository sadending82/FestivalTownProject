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
	int UID = 0;
	std::string AccountID = std::string(50, '\0');
	std::wstring NickName = L"Default";
	int UserLevel = 0;
	int PassLevel = 0;
	int UserTitle = 0;
	int ProfileSkin = 0;
	int Point = 0;
	std::tm date;
	int AttendanceDay = 0;
	sCharacterCustomizing characterCustomizing;
	int State;

	int Gold = 0;
	int Dia = 0;
	int Mileage = 0;
};


#pragma pack(pop) 