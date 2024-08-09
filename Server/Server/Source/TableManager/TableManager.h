#pragma once
#include <xlnt/xlnt.hpp>
#include <unordered_map>
#include "Tables/Tables.h"
#include "../utility.h"

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

	std::unordered_map<int, ItemTable>& getItemInfos() { return ItemInfos; }
	std::unordered_map<int, CharacterStat>& getCharacterStats() { return CharacterStats; }
	std::unordered_map<GameCode, FITH>& getFITH_Data() { return FITH_Data; }

private:
	std::unordered_map<int, ItemTable> ItemInfos;
	std::unordered_map<int, CharacterStat> CharacterStats;
	std::unordered_map<GameCode, FITH> FITH_Data;
};