#pragma once
#include "../../utility.h"
#include "../../Object/Player.h"
#include "../../TableManager/TableManager.h"
#include <unordered_set>

class Team {
public:
	Team();
	~Team();

	void Init(int lifeCount);

	void ReduceLife();
	void SetLife(int life);

	int GetLife() { return mLife.load(); }

	// �� ������� InGameID
	std::unordered_set<int>& GetMembers() { return mMembers; }

private:

	std::atomic<int> mLife;

	// �� ������� InGameID
	std::unordered_set<int> mMembers;
};