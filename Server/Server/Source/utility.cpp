#include "utility.h"

std::vector<uint8_t> MakeBuffer(const int type, const uint8_t* data, const int size)
{
	HEADER h;
	h.flatBufferSize = size;
	h.type = type;

	std::vector<uint8_t> buf(sizeof(HEADER) + size);
	memcpy(buf.data(), &h, sizeof(HEADER));
	memcpy(buf.data() + sizeof(HEADER), data, size);

	return buf;
}

Vector3f ConvertVec3fToVec2i(int x, int y)
{
	return Vector3f();
}

Vector3f ConvertVec2iToVec3f(int x, int y)
{
	Vector3f pos;

	pos.x = x * BLOCKSIZE + (BLOCKSIZE / 2);
	pos.y = y * BLOCKSIZE + (BLOCKSIZE / 2);
	pos.z = 0;

	return pos;
}
