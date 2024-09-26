#pragma once
#include <string>
#include <chrono>
// 테스트용 테이블

#pragma pack (push, 1)

enum ItemGrade {
	IG_Normal = 1,
	IG_Rare = 2,
	IG_Unique = 3,
};

enum ItemType {
	IT_Money = 1,
	IT_Skin = 10,
	IT_Accessory_Head = 21,
	IT_Accessory_Face = 22,
	IT_Accessory_Back = 23
};

enum ItemTable_Field {
	IT_Index,
	IT_Name,
	IT_File_Name,
	IT_Item_Type,
	IT_Item_Grade,
	IT_Open_Date,
	IT_Close_Date
};

struct ItemTable {
	int	Index;	
	std::string Name;	
	std::string File_Name;	
	ItemType Item_Type;
	ItemGrade Item_Grade;
	/*std::chrono::year_month_day Open_Date;
	std::chrono::year_month_day Close_Date;*/
};

#pragma pack(pop) 