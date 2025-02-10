#pragma once
#include "PacketManager.h"
#include "PacketProcessors/Processors.h"
#include "../Event/Event.h"
#include "../Server/Server.h"

#include <sstream>
#include <exception>

PacketManager::PacketManager(Server* server, PacketSender* packetSender)
{
    pServer = server;
    pPacketSender = packetSender;

    // packet
    {
        PacketProcessorMap[ePacketType::PING_CHECK] = std::make_unique<Packet_PingCheck>(pServer, pPacketSender);

        PacketProcessorMap[ePacketType::C2S_LOGIN_REQUEST] = std::make_unique<Packet_LoginRequest>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_SIGNUP_REQUEST] = std::make_unique<Packet_SignUpRequest>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_VERSION_CHECK_REQUEST] = std::make_unique<Packet_VersionCheckRequest>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_CHECK_ID_DUPLICATION] = std::make_unique<Packet_CheckID_Duplication>(pServer, pPacketSender);

        PacketProcessorMap[ePacketType::C2S_GACHA_REQUEST] = std::make_unique<Packet_GachaRequest>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_CURRENCY_AMOUNT_REQUEST] = std::make_unique<Packet_CurrencyAmountRequest>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_USER_ITEMS_REQUEST] = std::make_unique<Packet_UserItemsRequest>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_CHANGE_CHARACTER_CUSTOMIZING] = std::make_unique<Packet_ChangeCharacterCustomizing>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_ATTENDANCE_EVENT_REQUEST] = std::make_unique<Packet_AttendanceEventRequest>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_ATTENDANCE_REWARD_REQUEST] = std::make_unique<Packet_AttendanceRewardRequest>(pServer, pPacketSender);
        //PacketProcessorMap[ePacketType::C2S_EVENT_REWARD_REQUEST] = std::make_unique<Packet_EventRewardRequest>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_PASS_MISSION_STATE_REQUEST] = std::make_unique<Packet_UserMissionStateRequest>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_PASS_STATE_REQUEST] = std::make_unique<Packet_UserPassStateRequest>(pServer, pPacketSender);

        PacketProcessorMap[ePacketType::C2S_HEART_BEAT] = std::make_unique<Packet_HeartBeat>(pServer, pPacketSender);

        PacketProcessorMap[ePacketType::C2S_MATCHING_REQUEST] = std::make_unique<Packet_GameMatchingRequest>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_MATCHING_CANCEL] = std::make_unique<Packet_GameMatchingCancel>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_GAME_READY] = std::make_unique<Packet_GameReady>(pServer, pPacketSender);

        PacketProcessorMap[ePacketType::C2S_PLAYER_MOVE] = std::make_unique<Packet_PlayerMove>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_PLAYER_STOP] = std::make_unique<Packet_PlayerStop>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_PLAYER_SYNC] = std::make_unique<Packet_PlayerSync>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_PLAYER_GRAB_BOMB] = std::make_unique<Packet_PlayerGrabBomb>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_PLAYER_DROP_BOMB] = std::make_unique<Packet_PlayerDropBomb>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_PLAYER_THROW_BOMB] = std::make_unique<Packet_PlayerThrowBomb>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_PLAYER_GRAB_WEAPON] = std::make_unique<Packet_PlayerGrabWeapon>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_PLAYER_DROP_WEAPON] = std::make_unique<Packet_PlayerDropWeapon>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_PLAYER_ANIMATION] = std::make_unique<Packet_PlayerAnimation>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_PLAYER_DAMAGE_RECEIVE] = std::make_unique<Packet_PlayerDamageReceive>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_PLAYER_COLLISION_BLOCK] = std::make_unique<Packet_PlayerCollisionToBlock>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_PLAYER_GRAB_OTHER_PLAYER] = std::make_unique<Packet_PlayerGrabOtherPlayer>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_PLAYER_THROW_OTHER_PLAYER] = std::make_unique<Packet_PlayerThrowOtherPlayer>(pServer, pPacketSender);

        PacketProcessorMap[ePacketType::C2S_BOMB_INPUT] = std::make_unique<Packet_BombInput>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_BOMB_POS_SYNC] = std::make_unique<Packet_BombPositionSync>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_BOMB_EXPLOSION] = std::make_unique<Packet_BombExplosion>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_WEAPON_DELETE] = std::make_unique<Packet_WeaponDelete>(pServer, pPacketSender);

        PacketProcessorMap[ePacketType::C2S_ACCOUNT_RESET] = std::make_unique<Packet_AccountReset>(pServer, pPacketSender);
        PacketProcessorMap[ePacketType::C2S_GET_CURRENCY] = std::make_unique<Packet_GetCurrency>(pServer, pPacketSender);
    }


    // event
    {
        EventProcessorMap[eEventType::HEARTBEAT] = std::make_unique<Event_HeartBeat>(pServer, pPacketSender);

        EventProcessorMap[eEventType::BLOCKDROP] = std::make_unique<Event_BlockDrop>(pServer, pPacketSender);
        EventProcessorMap[eEventType::BOMBSPAWN] = std::make_unique<Event_BombSpawn>(pServer, pPacketSender);
        EventProcessorMap[eEventType::BOMBEXPLOSION] = std::make_unique<Event_BombExplosion>(pServer, pPacketSender);
        EventProcessorMap[eEventType::WEAPONSPAWN] = std::make_unique<Event_WeaponSpawn>(pServer, pPacketSender);

        EventProcessorMap[eEventType::GAMESTART] = std::make_unique<Event_GameStart>(pServer, pPacketSender);
        EventProcessorMap[eEventType::PLAYERRESPAWN] = std::make_unique<Event_PlayerRespawn>(pServer, pPacketSender);
        EventProcessorMap[eEventType::TIMESYNC] = std::make_unique<Event_RemainTimeSync>(pServer, pPacketSender);
        EventProcessorMap[eEventType::TIMEOVERCHECK] = std::make_unique<Event_TimeOverCheck>(pServer, pPacketSender);

        EventProcessorMap[eEventType::GROGGYRECOVERY] = std::make_unique<Event_GroggyRecovery>(pServer, pPacketSender);
    }
}

void PacketManager::ProcessPacket(unsigned char* data, const int key)
{

    HEADER* header = reinterpret_cast<HEADER*>(data);
    int type = header->type;
    int flatBufSize = header->flatBufferSize;
    int headerSize = sizeof(HEADER);

    if (PacketProcessorMap[type] == nullptr) {
        std::cout << "Received Invalid Packet Type : " << type << std::endl;
        return;
    }

	try {
        PacketProcessorMap[type]->Process(data + headerSize, header->flatBufferSize, key);
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

	EventProcessorMap[type]->Process(buf);
}
