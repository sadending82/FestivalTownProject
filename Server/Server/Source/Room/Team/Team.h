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

	void ReduceLife();
	void SetLife(int life);

	int GetLife() { return mLife.load(); }

private:

	std::atomic<int> mLife;

	std::unordered_map<int, Player*> mMembers;
};