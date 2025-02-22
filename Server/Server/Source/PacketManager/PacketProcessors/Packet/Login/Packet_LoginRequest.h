#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::LoginTable;

class Packet_LoginRequest : public PacketProcessor {

public:
	Packet_LoginRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			const LoginRequest* read = flatbuffers::GetRoot<LoginRequest>(data);

			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (player == nullptr) {
				return;
			}

			DB* pDB = pServer->GetDB();
			const char* id = read->account_id()->c_str();
			const char* pw = read->account_password()->c_str();

			std::pair<ERROR_CODE, UserInfo> result = pServer->UserLogin(id, pw, key);

			UserInfo userInfo = result.second;

			std::unordered_map<int, std::set<sDayAttendanceInfo>> attendanceInfoList;

			pServer->GetLobbyManager()->CheckAndLoadUserAttendanceEvent(player, attendanceInfoList);
			pServer->GetLobbyManager()->LoadMissionProgress(player);
			pServer->GetLobbyManager()->LoadPassState(player);
			bool ret = pServer->GetLobbyManager()->UpdateLoginMission(player);

			bool isNewEvent = pServer->GetLobbyManager()->CheckIsNewEvent(userInfo.LastLogin);

			pPacketSender->SendLoginResponse(key, result.first, userInfo, player->GetItems(), attendanceInfoList, isNewEvent);

			if (result.first == ERROR_CODE::ER_NONE) {
				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				player->SetUserInfoFromDB(userInfo);
				player->SetIsAuthenticated(true);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_LoginRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};