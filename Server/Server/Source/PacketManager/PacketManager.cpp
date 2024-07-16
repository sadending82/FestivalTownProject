#include "PacketManager.h"
#include "PacketProcessers/PacketProcessers.h"

void PacketManager::Init()
{
	PacketProcesserMap[TESTKEY] = std::make_unique<PacketTest>(&builder);
}

void PacketManager::ProcessPacket(int key, int type, const uint8_t* data, const int size)
{
	PacketProcesserMap[key]->Process(data, size);
}

std::vector<uint8_t> PacketManager::MakeBuffer(char type, const uint8_t* data, const int size)
{
	HEADER h;
	h.size = size;
	h.type = type;
	std::vector<uint8_t> buf(sizeof(HEADER) + size);
	memcpy(buf.data(), &h, sizeof(HEADER));
	memcpy(buf.data() + sizeof(HEADER), data, size);

	return buf;
}
