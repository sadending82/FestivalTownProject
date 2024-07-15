#pragma once
#include "flatbuffers/flatbuffers.h"
#include "test_generated.h"
#include "../DB/DB.h"

constexpr int MAXBUF = 100;

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
	vector<uint8_t> MakeBuffer(char type, const uint8_t* data, const int size);

private:
	flatbuffers::FlatBufferBuilder builder;
};

