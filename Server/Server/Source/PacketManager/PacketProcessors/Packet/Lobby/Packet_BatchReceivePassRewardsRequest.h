#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PassTable;

class Packet_BatchReceivePassRewardsRequest : public PacketProcessor {

public:
	Packet_BatchReceivePassRewardsRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (pServer->GetMode() == SERVER_MODE::LIVE && player->GetIsAuthenticated() == false) {
				pServer->Disconnect(key);
				return;
			}

			const BatchReceivePassRewardsRequest* read = flatbuffers::GetRoot<BatchReceivePassRewardsRequest>(data);

			TableManager* tableManager = pServer->GetTableManager();
			LobbyManager* lobbyManager = pServer->GetLobbyManager();

			int passIndex = read->pass_index();
			PassInfo& passInfo = tableManager->GetPassList()[passIndex];
			std::vector<ReceivedPassReward> rewards;
			int playerPassLevel = player->GetPassInfo()[passIndex].passState.passLevel;
			ePassType playerPassType = (ePassType)player->GetPassInfo()[passIndex].passState.passType;

			for (int i = 1; i <= playerPassLevel; ++i) {
				// 일반 보상
				bool result = lobbyManager->GivePassReward(player, passIndex, ePassType::PT_NORMAL, i);
				if (result == true) {
					PassLevel& passLevelInfo = tableManager->GetPassList()[passIndex].passLevelList[i][ePassType::PT_NORMAL];
					rewards.push_back(ReceivedPassReward{ result, passIndex, ePassType::PT_NORMAL, i, passLevelInfo.Reward_Item_Index, passLevelInfo.Reward_Item_Amount });
				}

				// 프리미엄 보상
				result = lobbyManager->GivePassReward(player, passIndex, ePassType::PT_PREMIUM, i);
				if (result == true) {
					PassLevel& passLevelInfo = tableManager->GetPassList()[passIndex].passLevelList[i][ePassType::PT_PREMIUM];
					rewards.push_back(ReceivedPassReward{ result, passIndex, ePassType::PT_PREMIUM, i, passLevelInfo.Reward_Item_Index, passLevelInfo.Reward_Item_Amount });
				}
			}


			//std::cout << rewards.size() << std::endl;

			pPacketSender->SendBatchReceivePassRewardsResponsePacket(key, passIndex, rewards);

		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_BatchReceivePassRewardsRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};