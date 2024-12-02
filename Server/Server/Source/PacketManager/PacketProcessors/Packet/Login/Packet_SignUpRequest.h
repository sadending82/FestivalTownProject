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

				ERROR_CODE result = pDB->InsertNewAcccount(id, pw);

				if (result == ERROR_CODE::ER_NONE) {
					int uid = pDB->InsertNewUser(id, nickName);
					if (uid == INVALIDKEY) {
						pDB->DeleteAcccount(id);
						result = ERROR_CODE::ER_DB_ERROR;
					}

					if (pDB->InsertUserGameRecords(uid) == ERROR_CODE::ER_DB_ERROR) {
						pDB->DeleteAcccount(id);
						pDB->DeleteUserInfo(uid);
						result = ERROR_CODE::ER_DB_ERROR;
					}
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