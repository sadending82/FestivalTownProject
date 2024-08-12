#pragma once
#include "PacketManager.h"
#include "PacketProcessors/Processors.h"
#include "../Event/Event.h"
#include <sstream>
#include <exception>

void PacketManager::Init(Server* server)
{
	pServer = server;

    PacketProcessorMap[ePacketType::C2S_HEARTBEAT] = std::make_unique<PacketHeartBeat>();
    PacketProcessorMap[ePacketType::C2S_PLAYERMOVE] = std::make_unique<PacketPlayerMove>();
    PacketProcessorMap[ePacketType::C2S_PLAYERSTOP] = std::make_unique<PacketPlayerStop>();
    PacketProcessorMap[ePacketType::C2S_PLAYERPOSSYNC] = std::make_unique<PacketPlayerPosSync>();
    PacketProcessorMap[ePacketType::C2S_BOMBINPUT] = std::make_unique<PacketBombInput>();

    EventProcessorMap[eEventType::HEARTBEAT] = std::make_unique<EventHeartBeat>();
    EventProcessorMap[eEventType::OBJECTDROP] = std::make_unique<EventObjectDrop>();
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
