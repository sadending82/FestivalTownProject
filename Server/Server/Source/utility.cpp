#include "utility.h"

std::vector<uint8_t> MakeBuffer(char type, const uint8_t* data, const int size)
{
	HEADER h;
	h.size = size;
	h.type = type;
	std::vector<uint8_t> buf(sizeof(HEADER) + size);
	memcpy(buf.data(), &h, sizeof(HEADER));
	memcpy(buf.data() + sizeof(HEADER), data, size);

	return buf;
}

std::vector<uint8_t> MakeEventBuffer(char type, const uint8_t* data, const int size, const int delaytime_ms)
{
	EVENT_HEADER h;
	h.size = size;
	h.type = type;
	h.start_time = std::chrono::system_clock::now() + std::chrono::milliseconds(delaytime_ms);
	std::vector<uint8_t> buf(sizeof(EVENT_HEADER) + size);
	memcpy(buf.data(), &h, sizeof(EVENT_HEADER));
	memcpy(buf.data() + sizeof(EVENT_HEADER), data, size);

	return buf;
}