#include "PacketManager.h"
#include "PacketProcessors/Processors.h"

void PacketManager::Init(Server* server)
{
	pServer = server;

	PacketProcessorMap[PACKETTYPE::C2S_TEST] = std::make_unique<PacketTest>();

	EventProcessorMap[PACKETTYPE::C2S_TEST] = std::make_unique<EventTest>();
}

void PacketManager::ProcessPacket(const int type, const uint8_t* data, const int size, const int key)
{
	PacketProcessorMap[type]->Process(pServer, data, size, key);
}

void PacketManager::ProcessEvent(const int type, const uint8_t* data, const int size)
{
	EventProcessorMap[type]->Process(pServer, data, size, 0);
}
