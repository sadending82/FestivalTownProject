#pragma once
#include <xlnt/xlnt.hpp>
#include <unordered_map>
#include "Tables/Tables.h"
#include "../utility.h"
#include "../Object/Map/Map.h"
#include "../protocol.h"

constexpr int variableNameIdx = 0;

class TableManager {
public:
	TableManager();
	~TableManager();

	void ClearAllTable();

	void ReadAllDataTable();

	void ReadItemTable();
	void ReadCharacterStat();
	void ReadFITHModeTable();
	void ReadMapData();
	void ReadWeaponStat();
	void ReadScoreConstantTable();

	std::unordered_map<int, ItemTable*>& GetItemInfos() { return ItemInfos; }
	std::unordered_map<int, CharacterStat*>& GetCharacterStats() { return CharacterStats; }
	std::unordered_map<int, WeaponStat*>& GetWeaponStats() { return WeaponStats; }
	std::unordered_map<GameMode, FITH*>& GetGameModeData() { return FITH_Data; }
	std::unordered_map<MapCode, Map*>& GetMapData() { return MapData; }
	std::unordered_map<GameMode, FITH_ScoreConstant*>& GetScoreConstantList() { return ScoreConstantList; }

	std::unordered_map<GameMode, std::vector<MapCode>>& getMapListByMode() { return MapListByMode; }

private:
	std::unordered_map<int, ItemTable*> ItemInfos;
	std::unordered_map<int, CharacterStat*> CharacterStats;
	std::unordered_map<int, WeaponStat*> WeaponStats;
	std::unordered_map<GameMode, FITH*> FITH_Data;
	std::unordered_map<MapCode, Map*> MapData;
	std::unordered_map<GameMode, FITH_ScoreConstant*> ScoreConstantList;

	std::unordered_map<GameMode, std::vector<MapCode>> MapListByMode;
};