#include "PacketManager.h"
#include "PacketProcessors/Processors.h"
#include "../Server/Server.h"

void PacketManager::Init(Server* server)
{
	pServer = server;

	PacketProcessorMap[PACKETTYPE::C2S_TEST] = std::make_unique<PacketTest>();
}

void PacketManager::ProcessPacket(const int type, const uint8_t* data, const int size, const int key)
{
	PacketProcessorMap[type]->Process(pServer->GetSessions(), data, size, key);
}
