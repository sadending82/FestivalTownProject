#pragma once
#include "PacketManager.h"
#include "PacketProcessors/Processors.h"
#include "../Event/Event.h"
#include <sstream>
#include <exception>

void PacketManager::Init(Server* server)
{
	pServer = server;

    // packet
    PacketProcessorMap[ePacketType::C2S_HEARTBEAT] = std::make_unique<PacketHeartBeat>();

    PacketProcessorMap[ePacketType::C2S_MATCHINGREQUEST] = std::make_unique<PacketGameMatchingRequest>();
    PacketProcessorMap[ePacketType::C2S_GAMEREADY] = std::make_unique<PacketGameReady>();

    PacketProcessorMap[ePacketType::C2S_PLAYERMOVE] = std::make_unique<PacketPlayerMove>();
    PacketProcessorMap[ePacketType::C2S_PLAYERSTOP] = std::make_unique<PacketPlayerStop>();
    PacketProcessorMap[ePacketType::C2S_PLAYERPOSSYNC] = std::make_unique<PacketPlayerPosSync>();
    PacketProcessorMap[ePacketType::C2S_PLAYERGRABBOMB] = std::make_unique<PacketPlayerGrabBomb>();
    PacketProcessorMap[ePacketType::C2S_PLAYERTHROWBOMB] = std::make_unique<PacketPlayerThrowBomb>();

    PacketProcessorMap[ePacketType::C2S_BOMBINPUT] = std::make_unique<PacketBombInput>();
    PacketProcessorMap[ePacketType::C2S_BOMBPOSSYNC] = std::make_unique<PacketBombPositionSync>();
    PacketProcessorMap[ePacketType::C2S_BOMBEXPLOSION] = std::make_unique<PacketBombExplosion>();


    // event
    EventProcessorMap[eEventType::HEARTBEAT] = std::make_unique<EventHeartBeat>();

    EventProcessorMap[eEventType::GAMEMATCHING] = std::make_unique<EventGameMatching>();

    EventProcessorMap[eEventType::BLOCKDROP] = std::make_unique<EventBlockDrop>();
    EventProcessorMap[eEventType::BOMBSPAWN] = std::make_unique<EventBombSpawn>();
    EventProcessorMap[eEventType::BOMBEXPLOSION] = std::make_unique<EventBombExplosion>();

    EventProcessorMap[eEventType::GAMESTART] = std::make_unique<EventGameStart>();
    EventProcessorMap[eEventType::PLAYERRESPAWN] = std::make_unique<EventPlayerRespawn>();
    EventProcessorMap[eEventType::TIMESYNC] = std::make_unique<EventRemainTimeSync>();
    EventProcessorMap[eEventType::TIMEOVERCHECK] = std::make_unique<EventTimeOverCheck>();
}

void PacketManager::ProcessPacket(unsigned char* data, const int key)
{
	//PacketProcessorMap[type]->Process(pServer, data, size, key);

    HEADER* header = reinterpret_cast<HEADER*>(data);
    int type = header->type;
    int flatBufSize = header->flatBufferSize;
    int headerSize = sizeof(HEADER);

    if (PacketProcessorMap[header->type] == nullptr) {
        std::cout << "Received Invalid Packet Type : " << type << std::endl;
        return;
    }

	try {
        PacketProcessorMap[type]->Process(pServer, data + headerSize, header->flatBufferSize, key);
	}
	catch (const std::exception& e) {
        std::cerr << "[ERROR] : " << e.what() << " Type : " << type << " KEY : " << key << std::endl;

        // ȣ�� ���� ����
        std::stringstream ss;
        ss << e.what();

        // ���� �޽������� ���� �̸��� ���� ��ȣ ����
        std::string line;
        while (getline(ss, line)) {
            if (line.find(":") != std::string::npos) {
                size_t pos = line.find(":");
                std::string file = line.substr(0, pos);
                int lineNum = stoi(line.substr(pos + 1));

                std::cerr << "  " << file << ":" << lineNum << std::endl;
            }
        }
	}
}

void PacketManager::ProcessEvent(const eEventType type, unsigned char* buf)
{
    if (EventProcessorMap[type] == nullptr) {
        std::cout << "Process Invalid Event Type : " << type << std::endl;
        return;
    }

	EventProcessorMap[type]->Process(pServer, buf);
}
