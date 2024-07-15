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

	std::unordered_map<int, ItemTable>& getItemInfos() { return ItemInfos; }

private:
	std::unordered_map<int, ItemTable> ItemInfos;
};