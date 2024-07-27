#pragma once
#include <xlnt/xlnt.hpp>
#include <unordered_map>
#include <array>
#include <iostream>
#include <memory>
#include "Tables/Tables.h"

class TableManager {
public:
	TableManager();
	~TableManager();

	void ClearAllTable();

	void ReadItemTable();
	void ReadCharacterStat();

	std::unordered_map<int, ItemTable>& getItemInfos() { return ItemInfos; }
	std::unordered_map<int, CharacterStat>& getCharacterStats() { return CharacterStats; }

private:
	std::unordered_map<int, ItemTable> ItemInfos;
	std::unordered_map<int, CharacterStat> CharacterStats;
};