#pragma once
#include "ObjectPool.h"

void ObjectPool::AddWeapon()
{
	mPool[nextID.load()] = std::make_shared<Weapon>(nextID.load(), eObjectType::WEAPON);
}

void ObjectPool::AddBomb()
{
	mPool[nextID.load()] = std::make_shared<Bomb>(nextID.load(), eObjectType::BOMB);
}

void ObjectPool::ObjectDeactive(int id)
{
	mPoolLock.lock_shared();
	mPool[id]->SetIsActive(false);
	mPoolLock.unlock_shared();
}

Object* ObjectPool::GetObjectByID(int id)
{
	mPoolLock.lock_shared();
	auto object = mPool[id];
	mPoolLock.unlock_shared();

	return object.get();
}

Object* ObjectPool::GetFreeObject(eObjectType type)
{
	mPoolLock.lock_shared();
	for (auto& obj : mPool) {
		auto object = obj.second;
		if (object->GetType() == type) {
			if (object->SetIsActive(true)) {
				mPoolLock.unlock_shared();
				return object.get();
			}
		}
	}
	mPoolLock.unlock_shared();

	std::shared_ptr<Object> newObject;

	switch (type) {
	case eObjectType::WEAPON: {
		newObject = std::make_shared<Weapon>(nextID.load(), eObjectType::WEAPON);
	}
							break;
	case eObjectType::BOMB: {
		newObject = std::make_shared<Bomb>(nextID.load(), eObjectType::BOMB);
	}
							break;
	}
	newObject->SetIsActive(true);

	mPoolLock.lock();
	mPool[nextID.load()] = newObject;
	nextID.fetch_add(1);
	mPoolLock.unlock();

	return newObject.get();
}

void ObjectPool::AllObjectDeactive()
{
	mPoolLock.lock();
	for (auto obj : mPool) {
		obj.second->SetIsActive(false);
	}
	mPoolLock.unlock();
}
