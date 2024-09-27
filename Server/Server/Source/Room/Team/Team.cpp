#include "Team.h"

Team::Team()
{
}

Team::~Team()
{
}

void Team::Init(int lifeCount)
{
	mLife = lifeCount;
	mMembers.clear();
}

void Team::ReduceLife()
{
	mLife.fetch_sub(1);
}

void Team::SetLife(int life)
{
	mLife.store(life);
}
