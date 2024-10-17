#pragma once
#include "PacketProcessors/PacketProcessor.h"
#include <unordered_map>

class PacketManager
{
public:
	PacketManager() {}
	PacketManager(Server* server, PacketSender* packetSender);
	~PacketManager() {}

	void ProcessPacket(unsigned char* data, const int key);
	void ProcessEvent(const eEventType type, unsigned char* buf);

private:
	std::unordered_map<int, std::unique_ptr<PacketProcessor>> PacketProcessorMap;
	std::unordered_map<int, std::unique_ptr<PacketProcessor>> EventProcessorMap;
	class Server* pServer = nullptr;
	class PacketSender* pPacketSender = nullptr;
};

