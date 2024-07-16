#include "PacketManager.h"
#include "PacketProcessors/Processors.h"

void PacketManager::Init()
{
	PacketProcessorMap[PACKETTYPE::C2S_TEST] = std::make_unique<PacketTest>();
}

void PacketManager::ProcessPacket(Session* session, int type, const uint8_t* data, const int size)
{
	PacketProcessorMap[type]->Process(session, data, size);
}
