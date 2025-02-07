#pragma once
#include <ctime>
#include "../../TableManager/Tables/Pass.h"
#pragma pack (push, 1)

constexpr int UserMission_Col_Count = 8;

struct UserMission{
	int user_UID = 0;
	int mission_code = 0;
	int mission_type = 0;
	int mission_group = 0;
	int mission_step = 0;
	int progress = 0;
	int required_count = 0;
	std::tm	assigned_date;
	int	is_rewarded = 0;

	void Init(int uid, int code, int type, int group, int step, int requiredCount) {
		user_UID = uid;
		mission_code = code;
		mission_type = type;
		mission_group = group;
		mission_step = step;
		progress = 0;
		required_count = requiredCount;
		is_rewarded = 0;
	}

	void Init(const int uid, const PassMissionInfo& missionInfo) {
		user_UID = uid;
		mission_code = missionInfo.index;
		mission_type = missionInfo.type;
		mission_group = missionInfo.mission_group;
		mission_step = missionInfo.mission_step;
		progress = 0;
		required_count = missionInfo.required_count;
		is_rewarded = 0;
	}
};

struct UserMissionList {
	// <type, <category, <group, <step, missionInfo>>>>
	std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, UserMission>>>> missionList;
};
#pragma pack(pop) 
