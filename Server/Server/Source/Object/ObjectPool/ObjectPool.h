#pragma once
#include "../Object.h"
#include "../Bomb/Bomb.h"
#include "../Weapon/Weapon.h"
#include <unordered_map>
#include <shared_mutex>
#include <memory>

class ObjectPool {
public:
	ObjectPool();
	~ObjectPool();

	std::unordered_map<int, Object*>& GetPool() { return mPool; }

	int GetNewObjectID();

	void AddWeapon();
	void AddBomb();

	void ObjectDeactive(int id);

	Object* GetObjectByID(int id);
	Object* GetFreeObject(eObjectType type);

	void AllObjectDeactive();

private:
	std::unordered_map<int, Object*> mPool;
	std::shared_mutex mPoolLock;
	std::atomic<int> nextID = 0;
};