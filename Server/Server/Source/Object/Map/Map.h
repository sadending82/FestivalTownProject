#pragma once
#include <vector>

class Map {

public:
	Map() {}
	~Map() {}

	std::vector<std::vector<int>>& GetStructure() { return Structure; }

private:
	std::vector<std::vector<int>> Structure;
};