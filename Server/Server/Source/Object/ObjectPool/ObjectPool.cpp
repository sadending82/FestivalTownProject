#pragma once
#include "ObjectPool.h"

ObjectPool::ObjectPool()
{
}

ObjectPool::~ObjectPool()
{
	for (auto obj : mPool) {
		delete obj.second;
	}
}

void ObjectPool::AddWeapon()
{
	mPool[nextID.load()] = new Weapon(nextID.load(), eObjectType::WEAPON);
}

void ObjectPool::AddBomb()
{
	mPool[nextID.load()] = new Bomb(nextID.load(), eObjectType::BOMB);
}

void ObjectPool::ObjectDeactive(int id)
{
	mPoolLock.lock_shared();
	mPool[id]->SetIsActive(false);
	mPoolLock.unlock_shared();
}

Object* ObjectPool::GetObjectByID(int id)
{
	Object* object = nullptr;
	mPoolLock.lock_shared();
	if (mPool.find(id) != mPool.end()) {
		object = mPool[id];
	}
	mPoolLock.unlock_shared();

	return object;
}

Object* ObjectPool::GetFreeObject(eObjectType type)
{
	mPoolLock.lock_shared();
	for (auto& obj : mPool) {
		auto object = obj.second;
		if (object->GetType() == type) {
			if (object->SetIsActive(true)) {
				mPoolLock.unlock_shared();
				return object;
			}
		}
	}
	mPoolLock.unlock_shared();

	Object* newObject{};

	switch (type) {
	case eObjectType::WEAPON: {
		newObject = new Weapon(nextID.load(), eObjectType::WEAPON);
	}
							break;
	case eObjectType::BOMB: {
		newObject = new Bomb(nextID.load(), eObjectType::BOMB);
	}
							break;
	}
	newObject->SetIsActive(true);

	mPoolLock.lock();
	mPool[nextID.load()] = newObject;
	nextID.fetch_add(1);
	mPoolLock.unlock();

	return newObject;
}

void ObjectPool::AllObjectDeactive()
{
	mPoolLock.lock();
	for (auto obj : mPool) {
		obj.second->SetIsActive(false);
	}
	mPoolLock.unlock();
}
