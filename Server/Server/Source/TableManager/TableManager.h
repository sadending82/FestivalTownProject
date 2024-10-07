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

	void Lock();
	void UnLock();

	std::unordered_map<INDEX, ItemTable>& GetItemInfos();

	std::unordered_map<INDEX, CharacterStat>& GetCharacterStats();

	std::unordered_map<INDEX, WeaponStat>& GetWeaponStats();
	std::unordered_map<GameMode, GameModeData>& GetGameModeData();
	std::unordered_map<MapCode, Map>& GetMapData();

	std::unordered_map<GameMode, PointConstants>& GetPointConstantList();

	std::unordered_map <GameMode, GameReward>& GetGameRewardList();
	// <GameMode, Point>
	std::unordered_map<GameMode, std::unordered_map<int, BonusReward>>& GetGameBonusRewardList();

	std::unordered_map<INDEX, RandomBox>& GetRandomBoxList();
	std::unordered_map<GACHA_GROUP, std::unordered_map<INDEX, GachaItem>>& GetGachaItemList();


	std::unordered_map<GameMode, std::vector<MapCode>>& getMapListByMode();

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

	std::atomic_flag mLockFlag = ATOMIC_FLAG_INIT;
	std::atomic<bool> mIsLoading;
};