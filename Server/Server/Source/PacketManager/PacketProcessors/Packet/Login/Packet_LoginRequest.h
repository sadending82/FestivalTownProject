#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::LoginTable;

class Packet_LoginRequest : public PacketProcessor {

public:
	Packet_LoginRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<LoginRequest>(nullptr)) {

				const LoginRequest* read = flatbuffers::GetRoot<LoginRequest>(data);

				DB* pDB = pServer->GetDB();
				const char* id = read->account_id()->c_str();
				const char* pw = read->account_password()->c_str();

				std::pair<bool, UserInfo> result = pServer->UserLogin(id, pw, key);

				UserInfo userInfo = result.second;

				pPacketSender->SendLoginResponse(key, result.first, userInfo.UID, userInfo.NickName, userInfo.Point, userInfo.Gold);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_LoginRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};