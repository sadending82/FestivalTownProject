#pragma once
#include "../../utility.h"
#include "../../Object/Player.h"
#include "../../TableManager/TableManager.h"
#include <unordered_map>

class Team {
public:
	Team();
	~Team();

	void Init(int lifeCount);

	void ReduceLife() { mLife--; }
	void SetLife(int life) { mLife = life; }

	int GetLife() { return mLife; }

private:

	int mLife;

	std::unordered_map<int, Player*> mMembers;
};