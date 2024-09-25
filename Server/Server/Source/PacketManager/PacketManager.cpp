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
        PacketProcessorMap[ePacketType::C2S_HEART_BEAT] = std::make_unique<Packet_HeartBeat>();

        PacketProcessorMap[ePacketType::C2S_MATCHING_REQUEST] = std::make_unique<Packet_GameMatchingRequest>();
        PacketProcessorMap[ePacketType::C2S_MATCHING_CANCEL] = std::make_unique<Packet_GameMatchingCancel>();
        PacketProcessorMap[ePacketType::C2S_GAME_READY] = std::make_unique<Packet_GameReady>();

        PacketProcessorMap[ePacketType::C2S_PLAYER_MOVE] = std::make_unique<Packet_PlayerMove>();
        PacketProcessorMap[ePacketType::C2S_PLAYER_STOP] = std::make_unique<Packet_PlayerStop>();
        PacketProcessorMap[ePacketType::C2S_PLAYER_SYNC] = std::make_unique<Packet_PlayerSync>();
        PacketProcessorMap[ePacketType::C2S_PLAYER_GRAB_BOMB] = std::make_unique<Packet_PlayerGrabBomb>();
        PacketProcessorMap[ePacketType::C2S_PLAYER_THROW_BOMB] = std::make_unique<Packet_PlayerThrowBomb>();
        PacketProcessorMap[ePacketType::C2S_PLAYER_GRAB_WEAPON] = std::make_unique<Packet_PlayerGrabWeapon>();
        PacketProcessorMap[ePacketType::C2S_PLAYER_DROP_WEAPON] = std::make_unique<Packet_PlayerDropWeapon>();
        PacketProcessorMap[ePacketType::C2S_PLAYER_ANIMATION] = std::make_unique<Packet_PlayerAnimation>();
        PacketProcessorMap[ePacketType::C2S_PLAYER_DAMAGE_RECEIVE] = std::make_unique<Packet_PlayerDamageReceive>();
        PacketProcessorMap[ePacketType::C2S_PLAYER_COLLISION_BLOCK] = std::make_unique<Packet_PlayerCollisionToBlock>();
        PacketProcessorMap[ePacketType::C2S_PLAYER_GRAB_OTHER_PLAYER] = std::make_unique<Packet_PlayerGrabOtherPlayer>();
        PacketProcessorMap[ePacketType::C2S_PLAYER_THROW_OTHER_PLAYER] = std::make_unique<Packet_PlayerThrowOtherPlayer>();

        PacketProcessorMap[ePacketType::C2S_BOMB_INPUT] = std::make_unique<Packet_BombInput>();
        PacketProcessorMap[ePacketType::C2S_BOMB_POS_SYNC] = std::make_unique<Packet_BombPositionSync>();
        PacketProcessorMap[ePacketType::C2S_BOMB_EXPLOSION] = std::make_unique<Packet_BombExplosion>();
        PacketProcessorMap[ePacketType::C2S_WEAPON_DELETE] = std::make_unique<Packet_WeaponDelete>();
    }


    // event
    {
        EventProcessorMap[eEventType::HEARTBEAT] = std::make_unique<Event_HeartBeat>();

        EventProcessorMap[eEventType::GAMEMATCHING] = std::make_unique<Event_GameMatching>();

        EventProcessorMap[eEventType::BLOCKDROP] = std::make_unique<Event_BlockDrop>();
        EventProcessorMap[eEventType::BOMBSPAWN] = std::make_unique<Event_BombSpawn>();
        EventProcessorMap[eEventType::BOMBEXPLOSION] = std::make_unique<Event_BombExplosion>();
        EventProcessorMap[eEventType::WEAPONSPAWN] = std::make_unique<Event_WeaponSpawn>();

        EventProcessorMap[eEventType::GAMESTART] = std::make_unique<Event_GameStart>();
        EventProcessorMap[eEventType::PLAYERRESPAWN] = std::make_unique<Event_PlayerRespawn>();
        EventProcessorMap[eEventType::TIMESYNC] = std::make_unique<Event_RemainTimeSync>();
        EventProcessorMap[eEventType::TIMEOVERCHECK] = std::make_unique<Event_TimeOverCheck>();

        EventProcessorMap[eEventType::GROGGYRECOVERY] = std::make_unique<Event_GroggyRecovery>();
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
