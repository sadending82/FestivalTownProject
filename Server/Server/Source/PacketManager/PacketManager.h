#pragma once
#include "PacketProcessors/PacketProcessor.h"
#include <unordered_map>

class PacketManager
{
public:
	PacketManager() {};
	~PacketManager() {};

	void Init();
	void ProcessPacket(Session* session, int type, const uint8_t* data, const int size);

private:
	std::unordered_map<int, std::unique_ptr<PacketProcessor>> PacketProcessorMap;
};

