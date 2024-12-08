#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::UtilitiesTable;

class Packet_PingCheck : public PacketProcessor {

public:
	Packet_PingCheck(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<PingCheck>(nullptr)) {
				const PingCheck* read = flatbuffers::GetRoot<PingCheck>(data);

				std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::PING_CHECK, data, size);
				pServer->GetSessions()[key]->DoSend(send_buffer.data(), send_buffer.size());
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_PingCheck ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};