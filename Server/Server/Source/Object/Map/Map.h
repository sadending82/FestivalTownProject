#pragma once
#include <vector>

class Map {

public:
	Map() {}
	~Map() {}

	std::vector<std::vector<char>>& GetStructure() { return Structure; }
	std::vector<std::pair<int, int>>& GetBombSpawnIndexes() { return BombSpawnIndexes; }
	std::vector<std::pair<int, int>>& GetBlockDropIndexes() { return BombSpawnIndexes; }
	std::vector<std::pair<int, int>>& GetPlayerSpawnIndexes(int team) { return PlayerSpawnIndexes[team]; }

private:
	std::vector<std::vector<char>> Structure;

	// first = x, second = y
	std::vector<std::pair<int, int>> BombSpawnIndexes;
	std::vector<std::pair<int, int>> BlockDropIndexes;
	std::unordered_map<int, std::vector<std::pair<int, int>>> PlayerSpawnIndexes;
};