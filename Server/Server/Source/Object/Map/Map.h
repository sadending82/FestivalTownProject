#pragma once
#include <vector>

class Map {

public:
	Map() {}
	~Map() {}

	std::vector<std::vector<char>>& GetStructure() { return Structure; }
	std::vector<std::pair<int, int>>& GetObjectSpawnIndexes() { return ObjectSpawnIndexes; }
	std::vector<std::pair<int, int>>& GetRedObjectSpawnIndexes() { return RedObjectSpawnIndexes; }
	std::vector<std::pair<int, int>>& GetBlueObjectSpawnIndexes() { return BlueObjectSpawnIndexes; }
	std::vector<std::pair<int, int>>& GetBlockDropIndexes() { return BlockDropIndexes; }
	std::vector<std::pair<int, int>>& GetPlayerSpawnIndexes(int team) { return PlayerSpawnIndexes[team]; }

private:
	std::vector<std::vector<char>> Structure;

	// first = x, second = y
	std::vector<std::pair<int, int>> ObjectSpawnIndexes;
	std::vector<std::pair<int, int>> RedObjectSpawnIndexes;
	std::vector<std::pair<int, int>> BlueObjectSpawnIndexes;
	std::vector<std::pair<int, int>> BlockDropIndexes;
	std::unordered_map<int, std::vector<std::pair<int, int>>> PlayerSpawnIndexes;
};