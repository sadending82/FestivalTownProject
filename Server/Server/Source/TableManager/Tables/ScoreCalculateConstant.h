#pragma once
#pragma pack (push, 1)

enum class FITH_ScoreConstant_Field {
	Kill_Point,
	Death_Point,
	Bomb_Point,
	Gold_Basic,
	Gold_Point,
	MVP_Gold_Point
};

struct FITH_ScoreConstant {
	float Kill_Point;
	float Death_Point;
	float Bomb_Point;
	float Gold_Basic;
	float Gold_Point;
	float MVP_Gold_Point;
};

#pragma pack(pop) 