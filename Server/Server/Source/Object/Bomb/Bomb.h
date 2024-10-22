#pragma once
#include "../Object.h"

class Bomb : public Object
{
public:
	Bomb();
	Bomb(int id, eObjectType type) : Object(id, type) {}
	~Bomb();

	void BombInit();

private:
};