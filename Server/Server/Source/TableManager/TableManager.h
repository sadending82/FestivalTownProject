#pragma once
#include <xlnt/xlnt.hpp>
#include <unordered_map>
#include "Tables/Tables.h"
#include "../utility.h"
#include "../Object/Map/Map.h"
#include "../protocol.h"

#define INDEX int
#define GACHA_GROUP int

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
	void ReadGachaTable();

	std::unordered_map<INDEX, ItemTable*>& GetItemInfos() { return ItemInfos; }
	std::unordered_map<INDEX, CharacterStat*>& GetCharacterStats() { return CharacterStats; }
	std::unordered_map<INDEX, WeaponStat*>& GetWeaponStats() { return WeaponStats; }
	std::unordered_map<GameMode, FITH*>& GetGameModeData() { return FITH_Data; }
	std::unordered_map<MapCode, Map*>& GetMapData() { return MapData; }
	std::unordered_map<GameMode, FITH_ScoreConstant*>& GetScoreConstantList() { return ScoreConstantList; }

	std::unordered_map<GameMode, std::vector<MapCode>>& getMapListByMode() { return MapListByMode; }

	std::unordered_map<INDEX, RandomBox*>& GetRandomBoxList(){return RandomBoxList;}
	std::unordered_map<GACHA_GROUP, std::unordered_map<INDEX, GachaItem*>>& GetGachaItemList() { return GachaItemList; }

private:
	std::unordered_map<INDEX, ItemTable*> ItemInfos;
	std::unordered_map<INDEX, CharacterStat*> CharacterStats;
	std::unordered_map<INDEX, WeaponStat*> WeaponStats;
	std::unordered_map<GameMode, FITH*> FITH_Data;
	std::unordered_map<MapCode, Map*> MapData;
	std::unordered_map<GameMode, FITH_ScoreConstant*> ScoreConstantList;

	std::unordered_map<INDEX, RandomBox*> RandomBoxList;
	std::unordered_map<GACHA_GROUP, std::unordered_map<INDEX, GachaItem*>> GachaItemList;

	std::unordered_map<GameMode, std::vector<MapCode>> MapListByMode;
};