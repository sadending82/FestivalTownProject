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
    {
        PacketProcessorMap[ePacketType::C2S_HEART_BEAT] = std::make_unique<PacketHeartBeat>();

        PacketProcessorMap[ePacketType::C2S_MATCHING_REQUEST] = std::make_unique<PacketGameMatchingRequest>();
        PacketProcessorMap[ePacketType::C2S_GAME_READY] = std::make_unique<PacketGameReady>();

        PacketProcessorMap[ePacketType::C2S_PLAYER_MOVE] = std::make_unique<PacketPlayerMove>();
        PacketProcessorMap[ePacketType::C2S_PLAYER_STOP] = std::make_unique<PacketPlayerStop>();
        PacketProcessorMap[ePacketType::C2S_PLAYER_POS_SYNC] = std::make_unique<PacketPlayerPosSync>();
        PacketProcessorMap[ePacketType::C2S_PLAYER_GRAB_BOMB] = std::make_unique<PacketPlayerGrabBomb>();
        PacketProcessorMap[ePacketType::C2S_PLAYER_THROW_BOMB] = std::make_unique<PacketPlayerThrowBomb>();
        PacketProcessorMap[ePacketType::C2S_PLAYER_GRAB_WEAPON] = std::make_unique<PacketPlayerGrabWeapon>();
        PacketProcessorMap[ePacketType::C2S_PLAYER_DROP_WEAPON] = std::make_unique<PacketPlayerDropWeapon>();
        PacketProcessorMap[ePacketType::C2S_PLAYER_ANIMATION] = std::make_unique<PacketPlayerAnimation>();
        PacketProcessorMap[ePacketType::C2S_PLAYER_DAMAGE_RECEIVE] = std::make_unique<PacketPlayerDamageReceive>();

        PacketProcessorMap[ePacketType::C2S_BOMB_INPUT] = std::make_unique<PacketBombInput>();
        PacketProcessorMap[ePacketType::C2S_BOMB_POS_SYNC] = std::make_unique<PacketBombPositionSync>();
        PacketProcessorMap[ePacketType::C2S_BOMB_EXPLOSION] = std::make_unique<PacketBombExplosion>();
    }


    // event
    {
        EventProcessorMap[eEventType::HEARTBEAT] = std::make_unique<EventHeartBeat>();

        EventProcessorMap[eEventType::GAMEMATCHING] = std::make_unique<EventGameMatching>();

        EventProcessorMap[eEventType::BLOCKDROP] = std::make_unique<EventBlockDrop>();
        EventProcessorMap[eEventType::BOMBSPAWN] = std::make_unique<EventBombSpawn>();
        EventProcessorMap[eEventType::BOMBEXPLOSION] = std::make_unique<EventBombExplosion>();
        EventProcessorMap[eEventType::WEAPONSPAWN] = std::make_unique<EventWeaponSpawn>();

        EventProcessorMap[eEventType::GAMESTART] = std::make_unique<EventGameStart>();
        EventProcessorMap[eEventType::PLAYERRESPAWN] = std::make_unique<EventPlayerRespawn>();
        EventProcessorMap[eEventType::TIMESYNC] = std::make_unique<EventRemainTimeSync>();
        EventProcessorMap[eEventType::TIMEOVERCHECK] = std::make_unique<EventTimeOverCheck>();
    }
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

        // 호출 스택 추적
        std::stringstream ss;
        ss << e.what();

        // 예외 메시지에서 파일 이름과 라인 번호 추출
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
