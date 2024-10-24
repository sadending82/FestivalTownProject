#pragma once
#include <string>

#define Mode_Out_Sheet 1
#define FITH_Mode_Sheet 2

enum class GameModeOut_Field {
	Player_Count = 3,
	Team_Count,
	Team_Color,
	Play_Map,
	Open_Date,
	Close_Date
};

enum class GameModeData_Field {
	Play_Time = 3,
	Player_Spawn_Time,
	Life_Count,
	Bomb_Spawn_Count,
	Bomb_Spawn_Time,
	Bomb_Delay_Time,
	Weapon1_Spawn_Index,
	Weapon1_Spawn_Count,
	Weapon1_Spawn_Time,
	Weapon2_Spawn_Index,
	Weapon2_Spawn_Count,
	Weapon2_Spawn_Time,
	Block1_Spawn_Index,
	Block1_Spawn_Count,
	Block1_Spawn_Time,
	Block2_Spawn_Index,
	Block2_Spawn_Count,
	Block2_Spawn_Time
};

#pragma pack (push, 1)
struct GameModeData {
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