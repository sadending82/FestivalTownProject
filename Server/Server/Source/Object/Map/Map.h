#pragma once
#include <vector>

enum FITH_MapBlock {
	FM_Normal = 'n',
	FM_ObjectSpwan = 'o',
	FM_Empty = 'x',
	FM_TeamRedHole = 'a',
	FM_TeamBlueHole = 'b',
	FM_TeamGreenHole = 'c',
	FM_TeamRedSpawn = '0',
	FM_TeamBlueSpawn = '1',
	FM_TeamGreenSpawn = '2'
};

class Map {

public:
	Map() {}
	Map(const Map& other) {
		this->mMapIndex = other.mMapIndex;
		this->Structure = other.Structure;
		this->ObjectSpawnIndexes = other.ObjectSpawnIndexes;
		this->BlockDropIndexes = other.BlockDropIndexes;
		this->PlayerSpawnIndexes = other.PlayerSpawnIndexes;
	}
	~Map() {}

	void SetMapIndex(int code) { mMapIndex = code; }

	int GetMapIndex() { return mMapIndex; }

	std::vector<std::vector<char>>& GetStructure() { return Structure; }
	std::vector<std::pair<int, int>>& GetObjectSpawnIndexes() { return ObjectSpawnIndexes; }
	std::vector<std::pair<int, int>>& GetBlockDropIndexes() { return BlockDropIndexes; }
	std::vector<std::pair<int, int>>& GetPlayerSpawnIndexes(int team) { return PlayerSpawnIndexes[team]; }

private:
	int mMapIndex;

	std::vector<std::vector<char>> Structure;

	// first = x, second = y
	std::vector<std::pair<int, int>> ObjectSpawnIndexes;
	std::vector<std::pair<int, int>> BlockDropIndexes;
	std::unordered_map<int, std::vector<std::pair<int, int>>> PlayerSpawnIndexes;
};