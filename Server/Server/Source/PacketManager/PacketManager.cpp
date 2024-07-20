#include "PacketManager.h"
#include "PacketProcessors/Processors.h"
#include <sstream>
#include <exception>

void PacketManager::Init(Server* server)
{
	pServer = server;

	PacketProcessorMap[PACKETTYPE::C2S_TEST] = std::make_unique<PacketTest>();

	EventProcessorMap[PACKETTYPE::C2S_TEST] = std::make_unique<EventTest>();
}

void PacketManager::ProcessPacket(const int type, const uint8_t* data, const int size, const int key)
{
	PacketProcessorMap[type]->Process(pServer, data, size, key);

	//try {
 //       PacketProcessorMap[type]->Process(pServer, data, size, key);
	//}
	//catch (const std::exception& e) {
 //       std::cerr << "[ERROR] : " << e.what() << " Type : " << type << " KEY : " << key << std::endl;

 //       // 호출 스택 추적
 //       std::stringstream ss;
 //       ss << e.what();

 //       // 예외 메시지에서 파일 이름과 라인 번호 추출
 //       std::string line;
 //       while (getline(ss, line)) {
 //           if (line.find(":") != std::string::npos) {
 //               size_t pos = line.find(":");
 //               std::string file = line.substr(0, pos);
 //               int lineNum = stoi(line.substr(pos + 1));

 //               std::cerr << "  " << file << ":" << lineNum << std::endl;
 //           }
 //       }
	//}
}

void PacketManager::ProcessEvent(const int type, const uint8_t* data, const int size)
{
	EventProcessorMap[type]->Process(pServer, data, size, 0);
}
