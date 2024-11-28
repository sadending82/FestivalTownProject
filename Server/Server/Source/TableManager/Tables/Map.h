#pragma once
#pragma pack (push, 1)

#define MapTheme_Sheet 0
#define MapData_Sheet 1

enum class MapTheme_Field {
	Map_Index,
	Map_Theme
};

struct MapTheme_Table {
	int Map_Index;
	int Map_Theme;
};

enum class MapData_Field {
	Map_Index,
	Mode_Index
};

struct MapData_Table {
	int Map_Index;
	int Mode_Index;
};

#pragma pack(pop) 