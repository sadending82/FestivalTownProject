#include "PacketManager.h"
#include "PacketProcessors/Processors.h"
#include "../Event/Event.h"
#include <sstream>
#include <exception>

void PacketManager::Init(Server* server)
{
	pServer = server;

    PacketProcessorMap[ePacketType::C2S_PLAYERMOVE] = std::make_unique<PacketPlayerMove>();
    PacketProcessorMap[ePacketType::C2S_PLAYERSTOP] = std::make_unique<PacketPlayerStop>();
    PacketProcessorMap[ePacketType::C2S_PLAYERPOSSYNC] = std::make_unique<PacketPlayerPosSync>();

    EventProcessorMap[eEventType::PLAYERPOSSYNC] = std::make_unique<EventPlayerPosSync>();
}

void PacketManager::ProcessPacket(const int type, const uint8_t* data, const int size, const int key)
{
	//PacketProcessorMap[type]->Process(pServer, data, size, key);

    if (PacketProcessorMap[type] == nullptr) {
        std::cout << "�̻��Ѱ� �� : " << type << std::endl;
        return;
    }

	try {
        PacketProcessorMap[type]->Process(pServer, data, size, key);
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
	EventProcessorMap[type]->Process(pServer, buf);
}
