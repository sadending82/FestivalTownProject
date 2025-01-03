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

int ObjectPool::GetNewObjectID()
{
	int newObjectID = nextID.load();
	while (!nextID.compare_exchange_weak(newObjectID, newObjectID + 1)) {

	}
	return newObjectID;
}

void ObjectPool::AddWeapon()
{
	int newWeaponID = nextID.load();
	mPool[newWeaponID] = new Weapon(newWeaponID, eObjectType::WEAPON);
}

void ObjectPool::AddBomb()
{
	int newBombID = nextID.load();
	mPool[newBombID] = new Bomb(newBombID, eObjectType::BOMB);
}

void ObjectPool::ObjectDeactive(int id)
{
	mPoolLock.lock_shared();
	if (mPool.find(id) != mPool.end()) {
		mPool[id]->SetIsActive(false);
	}
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

	int newObjID = GetNewObjectID();

	switch (type) {
	case eObjectType::WEAPON: {
		newObject = new Weapon(newObjID, eObjectType::WEAPON);
	}
							break;
	case eObjectType::BOMB: {
		newObject = new Bomb(newObjID, eObjectType::BOMB);
	}
							break;
	}
	newObject->SetIsActive(true);

	mPoolLock.lock();
	mPool[newObjID] = newObject;
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
