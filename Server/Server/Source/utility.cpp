#include "utility.h"
#include "Object/Player.h"
#include <vector>
#include <cmath>

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

Vector3f ConvertVec3fToVec2i(Vector3f vec)
{
	float x = vec.x;
	float z = vec.z;

	return Vector3f(std::trunc(x / BLOCKSIZE), 0, std::trunc(z / BLOCKSIZE));
}

Vector3f ConvertVec2iToVec3f(int x, int z)
{
	Vector3f pos;

	pos.x = x * BLOCKSIZE + (BLOCKSIZE / 2);
	pos.y = 0;
	pos.z = z * BLOCKSIZE + (BLOCKSIZE / 2);

	return pos;
}

std::string wstringToString(const std::wstring& input)
{
	int size = WideCharToMultiByte(CP_UTF8, 0, input.c_str(), -1, NULL, 0, NULL, NULL);
	std::string utf8_string(size, 0);
	WideCharToMultiByte(CP_UTF8, 0, input.c_str(), -1, &utf8_string[0], size, NULL, NULL);
	return utf8_string;
}
