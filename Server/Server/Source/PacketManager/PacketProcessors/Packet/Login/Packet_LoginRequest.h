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

				std::pair<ERROR_CODE, UserInfo> result = pServer->UserLogin(id, pw, key);

				UserInfo userInfo = result.second;

				std::unordered_map<int, std::set<sDayAttendanceInfo>> attendanceInfoList;

				pServer->GetLobbyManager()->CheckAndLoadUserAttendanceEvent(userInfo.UID, attendanceInfoList);

				bool isNewEvent = pServer->GetLobbyManager()->CheckIsNewEvent(userInfo.date);
				pPacketSender->SendLoginResponse(key, result.first, userInfo, attendanceInfoList, isNewEvent);

				if (result.first == ERROR_CODE::ER_NONE) {
					Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
					player->SetUserInfoFromDB(userInfo);
					player->SetIsAuthenticated(true);
				}
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_LoginRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};