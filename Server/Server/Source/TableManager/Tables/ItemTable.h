#pragma once
#include <string>
#include <chrono>
// 테스트용 테이블

#pragma pack (push, 1)

enum class ItemGrade {
	Normal = 1,
	Rare = 2,
	Unique = 3,
};

enum class ItemType {
	Money = 1,
	Skin = 10,
	Accessory_Head = 21,
	Accessory_Face = 22,
	Accessory_Back = 23
};

enum class ItemTable_Field {
	Index,
	Name,
	File_Name,
	Item_Type,
	Item_Grade,
	Open_Date,
	Close_Date
};

struct ItemTable {
	int	Index;	
	std::string Name;	
	std::string File_Name;	
	ItemType Item_Type;
	ItemGrade Item_Grade;
	std::tm Open_Date;
	std::tm Close_Date;
};

#pragma pack(pop) 