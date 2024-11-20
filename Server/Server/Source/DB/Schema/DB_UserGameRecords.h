#pragma once
#include <atomic>

#pragma pack (push, 1)

enum class UserGameRecords_Field {
		UID = 1,
		KillCount,
		DeathCount,
		Point,
		Weapon_Kill_Count,
		Punch_Kill_Count,
		Bomb_Count,
		Groggy_Count,
		Pick_Weapon_Count,
		Pick_Bomb_Count,
		Battle_Count,
		FITH_Team_Count,
		FITH_Indiv_Count,
		Victory_Count,
		Gacha_Count,
		Change_Skin_Count,
		Buy_Box_Count,
		Login_Count,
		Play_Time_Count
};


struct UserGameRecords {
	int UID = 0;
	std::atomic<int> KillCount = 0;
	std::atomic<int> DeathCount = 0;
	std::atomic<int> Point = 0;
	std::atomic<int> Weapon_Kill_Count = 0;
	std::atomic<int> Punch_Kill_Count = 0;
	std::atomic<int> Bomb_Count = 0;
	std::atomic<int> Groggy_Count = 0;
	std::atomic<int> Pick_Weapon_Count = 0;
	std::atomic<int> Pick_Bomb_Count = 0;
	std::atomic<int> Battle_Count = 0;
	std::atomic<int> FITH_Team_Count = 0;
	std::atomic<int> FITH_Indiv_Count = 0;
	std::atomic<int> Victory_Count = 0;
	std::atomic<int> Gacha_Count = 0;
	std::atomic<int> Change_Skin_Count = 0;
	std::atomic<int> Buy_Box_Count = 0;
	std::atomic<int> Login_Count = 0;
	std::atomic<int> Play_Time_Count = 0;


	void Init() {
		UID = -1;
		KillCount.store(0);
		DeathCount.store(0);
		Point.store(0);
		Weapon_Kill_Count.store(0);
		Punch_Kill_Count.store(0);
		Bomb_Count.store(0);
		Groggy_Count.store(0);
		Pick_Weapon_Count.store(0);
		Pick_Bomb_Count.store(0);
		Battle_Count.store(0);
		FITH_Team_Count.store(0);
		FITH_Indiv_Count.store(0);
		Victory_Count.store(0);
		Gacha_Count.store(0);
		Change_Skin_Count.store(0);
		Buy_Box_Count.store(0);
		Login_Count.store(0);
		Play_Time_Count.store(0);
	}
};



#pragma pack(pop) 