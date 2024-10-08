#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::LoginTable;

class Packet_SignUpRequest : public PacketProcessor {

public:
	Packet_SignUpRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {

			mBuilder.Clear();

			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<SignUpRequest>(nullptr)) {

				const SignUpRequest* read = flatbuffers::GetRoot<SignUpRequest>(data);

				pPacketSender->SendSignUpResponse(key, 0);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};