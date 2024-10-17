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

				bool result = pDB->InsertNewAcccount(read->account_id()->c_str(), read->account_password()->c_str());

				if (result == true) {
					pDB->InsertNewUser(read->account_id()->c_str(), read->nickname()->c_str());
				}
				else {
					//rollback (Delete Account)
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