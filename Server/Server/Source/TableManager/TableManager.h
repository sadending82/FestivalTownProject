#pragma once
#include <iostream>
#include <xlnt/xlnt.hpp>
#include <unordered_map>
#include "Tables/Tables.h"
#include "../utility.h"
#include "../Object/Map/Map.h"
#include "../protocol.h"

#define INDEX int
#define GACHA_GROUP int

inline constexpr int variableNameIdx = 0;

class TableManager {
public:
	TableManager();
	~TableManager();

	void ClearAllTable();

	void ReadAllDataTable();

	void ReadItemTable();
	void ReadCharacterStat();
	void ReadGameModeTable();
	void ReadMapData();
	void ReadWeaponStat();
	void ReadPointConstantTable();

	void ReadGameReward();

	void ReadGachaTable();

	std::unordered_map<INDEX, ItemTable>& GetItemInfos() { return ItemInfos; }

	std::unordered_map<INDEX, CharacterStat>& GetCharacterStats() { return CharacterStats; }

	std::unordered_map<INDEX, WeaponStat>& GetWeaponStats() { return WeaponStats; }
	std::unordered_map<GameMode, GameModeData>& GetGameModeData() { return GameModeDatas; }
	std::unordered_map<MapCode, Map>& GetMapData() { return MapData; }

	std::unordered_map<GameMode, PointConstants>& GetPointConstantList() { return PointConstantList; }

	std::unordered_map <GameMode, GameReward>& GetGameRewardList() { return GameRewardList; }
	// <GameMode, Point>
	std::unordered_map<GameMode, std::unordered_map<int, BonusReward>>& GetGameBonusRewardList() { return GameBonusRewardList; }

	std::unordered_map<INDEX, RandomBox>& GetRandomBoxList(){return RandomBoxList;}
	std::unordered_map<GACHA_GROUP, std::unordered_map<INDEX, GachaItem>>& GetGachaItemList() { return GachaItemList; }


	std::unordered_map<GameMode, std::vector<MapCode>>& getMapListByMode() { return MapListByMode; }

private:
	xlnt::workbook mWorkbook;
	xlnt::worksheet mWorksheet;

	std::unordered_map<INDEX, ItemTable> ItemInfos;

	std::unordered_map<INDEX, CharacterStat> CharacterStats;
	std::unordered_map<INDEX, AttackStat> AttackStats;
	std::unordered_map<INDEX, MoveStat> MoveStats;

	std::unordered_map<INDEX, WeaponStat> WeaponStats;
	std::unordered_map<GameMode, GameModeData> GameModeDatas;
	std::unordered_map<MapCode, Map> MapData;
	std::unordered_map<GameMode, PointConstants> PointConstantList;
	

	std::unordered_map<GameMode, GameReward> GameRewardList;
	// <GameMode, Point>
	std::unordered_map<GameMode, std::unordered_map<int, BonusReward>> GameBonusRewardList;

	std::unordered_map<INDEX, RandomBox> RandomBoxList;
	std::unordered_map<GACHA_GROUP, std::unordered_map<INDEX, GachaItem>> GachaItemList;

	std::unordered_map<GameMode, std::vector<MapCode>> MapListByMode;
};