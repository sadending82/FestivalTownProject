#pragma once
#include <string>

enum FITH_Field {
	Play_Time,
	Player_Spawn_Time,
	Team_Life_Count,
	Bomb_Spawn_Count,
	Bomb_Spawn_Time,
	Bomb_Delay_Time,
	Bomb_Spawn_Location_MinX,
	Bomb_Spawn_Location_MaxX,
	Bomb_Spawn_Location_MinY,
	Bomb_Spawn_Location_MaxY,
	Weapon_Spawn_Time,
	Weapon_Spawn_Count,
	Block_Spawn_Count,
	Block_Spawn_Time,
	Block_Spawn_Location_MinX,
	Block_Spawn_Location_MaxX,
	Block_Spawn_Location_MinY,
	Block_Spawn_Location_MaxY
};

#pragma pack (push, 1)
struct FITH {
	int Play_Time;
	int Player_Spawn_Time;
	int Team_Life_Count;
	int Bomb_Spawn_Count;
	int Bomb_Spawn_Time;
	int Bomb_Delay_Time;
	float Bomb_Spawn_Location_MinX;
	float Bomb_Spawn_Location_MaxX;
	float Bomb_Spawn_Location_MinY;
	float Bomb_Spawn_Location_MaxY;
	int Weapon_Spawn_Time;
	int Weapon_Spawn_Count;
	int Block_Spawn_Count;
	int Block_Spawn_Time;
	float Block_Spawn_Location_MinX;
	float Block_Spawn_Location_MaxX;
	float Block_Spawn_Location_MinY;
	float Block_Spawn_Location_MaxY;
};

#pragma pack(pop) 