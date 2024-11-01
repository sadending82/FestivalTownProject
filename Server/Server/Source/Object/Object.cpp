#include "Object.h"

Object::Object()
{
	mIsGrabbed = false;
}

Object::~Object()
{
}

void Object::SetUniqueCode()
{
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	long long millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

	int numDigits = log10(mID) + 1;
	mUniqueCode = millis * pow(10, numDigits) + mID;
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
