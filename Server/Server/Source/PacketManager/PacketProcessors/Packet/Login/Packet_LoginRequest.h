#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::LoginTable;

class Packet_LoginRequest : public PacketProcessor {

public:
	Packet_LoginRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {

			mBuilder.Clear();

			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<LoginRequest>(nullptr)) {

				const LoginRequest* read = flatbuffers::GetRoot<LoginRequest>(data);

				DB* pDB = pServer->GetDB();

				bool result = pDB->CheckValidateLogin(read->account_id()->c_str(), read->account_password()->c_str());

				pPacketSender->SendLoginResponse(key, result);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};