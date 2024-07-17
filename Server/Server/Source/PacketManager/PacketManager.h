#pragma once
#include "PacketProcessors/PacketProcessor.h"
#include <unordered_map>

class Server;

class PacketManager
{
public:
	PacketManager() {};
	~PacketManager() {};

	void Init(Server* server);
	void ProcessPacket(const int type, const uint8_t* data, const int size, const int key);

private:
	std::unordered_map<int, std::unique_ptr<PacketProcessor>> PacketProcessorMap;
	Server* pServer = nullptr;
};

