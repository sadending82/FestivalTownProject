#pragma once
#include "PacketProcessers/PacketProcesser.h"
#include <unordered_map>

constexpr int MAXBUF = 100;

constexpr int TESTKEY = 1;

#pragma pack (push, 1)

struct HEADER {
public:
	uint16_t	size;
	uint16_t	type;
};
#pragma pack(pop)

class PacketManager
{
public:
	PacketManager() {};
	~PacketManager() {};

	void Init();
	void ProcessPacket(int key, int type, const uint8_t* data, const int size);
	std::vector<uint8_t> MakeBuffer(char type, const uint8_t* data, const int size);

private:
	flatbuffers::FlatBufferBuilder builder;

	std::unordered_map<int, std::unique_ptr<PacketProcesser>> PacketProcesserMap;
};

