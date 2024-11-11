#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::LoginTable;

class Packet_SignUpRequest : public PacketProcessor {

public:
	Packet_SignUpRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<SignUpRequest>(nullptr)) {

				const SignUpRequest* read = flatbuffers::GetRoot<SignUpRequest>(data);

				DB* pDB = pServer->GetDB();

				const char* id = read->account_id()->c_str();
				const char* pw = read->account_password()->c_str();
				const char* nickName = read->nickname()->c_str();

				bool result = pDB->InsertNewAcccount(id, pw);

				if (result == true) {
					pDB->InsertNewUser(id, nickName);
				}
				else {
					pDB->DeleteAcccount(id);
				}

				pPacketSender->SendSignUpResponse(key, result);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_SignUpRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};