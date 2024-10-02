#pragma once
#include <string>

#define Mode_Out_Sheet 0
#define FITH_Mode_Sheet 1

enum class GameModeOut_Field {
	Player_Count = 3,
	Team_Count,
	Team_Color,
	Play_Map,
	Open_Date,
	Close_Date
};

enum GameModeInfo_Field {
	GM_Play_Time = 3,
	GM_Player_Spawn_Time,
	GM_Life_Count,
	GM_Bomb_Spawn_Count,
	GM_Bomb_Spawn_Time,
	GM_Bomb_Delay_Time,
	GM_Weapon1_Spawn_Index,
	GM_Weapon1_Spawn_Count,
	GM_Weapon1_Spawn_Time,
	GM_Weapon2_Spawn_Index,
	GM_Weapon2_Spawn_Count,
	GM_Weapon2_Spawn_Time,
	GM_Block1_Spawn_Index,
	GM_Block1_Spawn_Count,
	GM_Block1_Spawn_Time,
	GM_Block2_Spawn_Index,
	GM_Block2_Spawn_Count,
	GM_Block2_Spawn_Time
};

#pragma pack (push, 1)
struct GameModeInfo {
	int Player_Count;
	int Team_Count;
	int	Team_Color;
	int	Play_Map;
	std::tm Open_Date;
	std::tm Close_Date;

	int Play_Time;
	int Player_Spawn_Time;
	int Life_Count;
	int Bomb_Spawn_Count;
	int Bomb_Spawn_Time;
	int Bomb_Delay_Time;
	int Weapon1_Spawn_Index;
	int Weapon1_Spawn_Count;
	int Weapon1_Spawn_Time;
	int Weapon2_Spawn_Index;
	int Weapon2_Spawn_Count;
	int Weapon2_Spawn_Time;
	int Block1_Spawn_Index;	
	int Block1_Spawn_Count;	
	int Block1_Spawn_Time;	
	int Block2_Spawn_Index;	
	int Block2_Spawn_Count;	
	int Block2_Spawn_Time;

};

#pragma pack(pop) 