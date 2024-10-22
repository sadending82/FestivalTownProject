#include "Object.h"

Object::Object()
{
	mIsGrabbed = false;
}

Object::~Object()
{
}

bool Object::SetIsGrabbed(bool desired)
{
	bool expected = !desired;
	return mIsGrabbed.compare_exchange_strong(expected, desired);
}

bool Object::SetIsActive(bool desired)
{
	bool expected = !desired;
	return mIsActive.compare_exchange_strong(expected, desired);
}
