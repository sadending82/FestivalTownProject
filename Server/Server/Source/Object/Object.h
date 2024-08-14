#pragma once
#include "../utility.h"
class Object {

public:
	Object() {}
	virtual ~Object() {}

	int GetID() { return mID; }
	Vector3f GetPosition() { return mPosition; }
	Vector3f GetDirection() { return mDirection; }

	void SetID(int id) { mID = id; }
	void SetPosition(float x, float y, float z) { mPosition = Vector3f(x, y, z); }
	void SetPosition(Vector3f v3f) { mPosition = v3f; }
	void SetDirection(float x, float y, float z) { mDirection = Vector3f(x, y, z); }
	void SetDirection(Vector3f v3f) { mDirection = v3f; }

protected:
	int			mID;
	Vector3f	mPosition;
	Vector3f	mDirection;
};