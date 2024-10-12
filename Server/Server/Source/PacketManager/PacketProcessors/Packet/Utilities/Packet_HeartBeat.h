#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::UtilitiesTable;

class Packet_HeartBeat : public PacketProcessor {

public:
	Packet_HeartBeat(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {

			std::cout << key << " HeartBeat\n";

			mBuilder.Clear();

			// 지금은 버퍼 내용은 사용 X 유효한 버퍼인지만 확인해서 처리
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<HeartBeat>(nullptr)) {
				const HeartBeat* read = flatbuffers::GetRoot<HeartBeat>(data);

				pServer->GetSessions()[key]->SetIsHeartbeatAck(true);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_HeartBeat ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};