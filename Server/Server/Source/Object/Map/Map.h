#pragma once
#include <vector>

enum FITH_MapBlock {
	FM_Empty = 'X',
	FM_Normal = 'n',
	FM_BombHole = 'h',
	FM_ObjectSpwan = 'o',
	FM_TeamRedSpawn = '0',
	FM_TeamBlueSpawn = '1',
	FM_TeamGreenSpawn = '2'
};

class Map {

public:
	Map() {}
	Map(const Map& other) {
		this->mMapCode = other.mMapCode;
		this->Structure = other.Structure;
		this->ObjectSpawnIndexes = other.ObjectSpawnIndexes;
		this->BlockDropIndexes = other.BlockDropIndexes;
		this->PlayerSpawnIndexes = other.PlayerSpawnIndexes;
	}
	~Map() {}

	void SetMapCode(MapCode code) { mMapCode = code; }

	MapCode GetMapCode() { return mMapCode; }

	std::vector<std::vector<char>>& GetStructure() { return Structure; }
	std::vector<std::pair<int, int>>& GetObjectSpawnIndexes() { return ObjectSpawnIndexes; }
	std::vector<std::pair<int, int>>& GetBlockDropIndexes() { return BlockDropIndexes; }
	std::vector<std::pair<int, int>>& GetPlayerSpawnIndexes(int team) { return PlayerSpawnIndexes[team]; }

private:
	MapCode mMapCode;

	std::vector<std::vector<char>> Structure;

	// first = x, second = y
	std::vector<std::pair<int, int>> ObjectSpawnIndexes;
	std::vector<std::pair<int, int>> BlockDropIndexes;
	std::unordered_map<int, std::vector<std::pair<int, int>>> PlayerSpawnIndexes;
};