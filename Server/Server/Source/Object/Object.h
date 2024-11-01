#pragma once
#include "../utility.h"
#include "../protocol.h"
#include <memory>

// Bomb, Weapon ...
class Object {

public:
	Object();
	Object(int id, eObjectType type) : mID(id), mType(type), mIsActive(false) {};
	virtual ~Object();

	eObjectType GetType() { return mType; }
	std::atomic<bool>	GetIsActive() { return mIsActive.load(); }
	int			GetID() { return mID; }
	Vector3f	GetPosition() { return mPosition; }
	Vector3f	GetDirection() { return mDirection; }
	bool		GetIsGrabbed() { return mIsGrabbed.load(); }
	int			GetOwenrID() { return mOwnerID; }
	long long	GetUniqueCode() { return mUniqueCode; }

	void		SetID(int id) { mID = id; }
	void		SetPosition(float x, float y, float z) { mPosition = Vector3f(x, y, z); }
	void		SetPosition(Vector3f v3f) { mPosition = v3f; }
	void		SetDirection(float x, float y, float z) { mDirection = Vector3f(x, y, z); }
	void		SetDirection(Vector3f v3f) { mDirection = v3f; }
	void		SetOwenrID(int playerid) { mOwnerID = playerid; }

	// cas
	bool		SetIsGrabbed(bool desired);
	bool		SetIsActive(bool desired);

protected:
	eObjectType mType;
	std::atomic<bool> mIsActive;
	int			mID = INVALIDKEY;
	Vector3f	mPosition;
	Vector3f	mDirection;
	std::atomic<bool> mIsGrabbed;
	int			mOwnerID = INVALIDKEY;

	long long	mUniqueCode = 0;
};