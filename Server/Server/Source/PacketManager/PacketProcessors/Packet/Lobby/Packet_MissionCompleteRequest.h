#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PassTable;

class Packet_MissionCompleteRequest : public PacketProcessor {

public:
	Packet_MissionCompleteRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (pServer->GetMode() == SERVER_MODE::LIVE && player->GetIsAuthenticated() == false) {
				pServer->Disconnect(key);
				return;
			}

			const MissionCompleteRequest* read = flatbuffers::GetRoot<MissionCompleteRequest>(data);

			TableManager* tableManager = pServer->GetTableManager();
			LobbyManager* lobbyManager = pServer->GetLobbyManager();

			// юс╫ц
			int mission_index = read->mission_index();

			PassMissionInfo& missionInfo = tableManager->GetPassMissionDataListByIndex()[mission_index];

			int pass_index = missionInfo.pass_index;
			bool result = false;
			if (lobbyManager->CheckCompleteMission(player, mission_index)) {
				result = true;
			}

			PlayerPassInfo& playerPassInfo = player->GetPassInfo()[pass_index];
			auto& playerMissionList = player->GetMissionList().missionList[pass_index][missionInfo.type][missionInfo.mission_category][missionInfo.mission_group];
			UserMission& completeMission = playerMissionList[missionInfo.mission_step];

			pPacketSender->SendMissionCompleteResponsePacket(key, result, playerPassInfo, completeMission, missionInfo.reward_item, missionInfo.reward_item_amount);
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_MissionCompleteRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};