#pragma once
#include <vector>

class Map {

public:
	Map() {}
	~Map() {}

	std::vector<std::vector<char>>& GetStructure() { return Structure; }
	std::vector<std::pair<int, int>>& GetBombSpawnIndexes() { return BombSpawnIndexes; }
	std::vector<std::pair<int, int>>& GetBlockDropIndexes() { return BlockDropIndexes; }
	std::vector<std::pair<int, int>>& GetPlayerSpawnIndexes() { return PlayerSpawnIndexes; }

private:
	std::vector<std::vector<char>> Structure;

	// first = x, second = y
	std::vector<std::pair<int, int>> BombSpawnIndexes;
	std::vector<std::pair<int, int>> BlockDropIndexes;
	std::vector<std::pair<int, int>> PlayerSpawnIndexes;
};