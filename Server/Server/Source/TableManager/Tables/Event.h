#pragma once
#include <string>

#pragma pack (push, 1)

#define Evnet_Main_Sheet 0
#define Evnet_List_Sheet 1


enum class Event_Main_Field {
	Event_ID,
	Name,
	Type,
	Open_Date,
	Close_Date,
};

struct Event_Main {
	int Event_ID;
	std::string Name;
	int Type;
	std::tm Open_Date;
	std::tm Close_Date;
};

enum class Event_List_Field {
	Index,
	Event_List,
	Day,
	Reward_Item_Index,
	Reward_Item_Value,
	Open_Date,
	Close_Date,
};

struct Event_List {
	int Index;
	int Event_List;
	int Day;
	int Reward_Item_Index;
	int Reward_Item_Value;
	std::tm Open_Date;
	std::tm Close_Date;
};


#pragma pack(pop) 