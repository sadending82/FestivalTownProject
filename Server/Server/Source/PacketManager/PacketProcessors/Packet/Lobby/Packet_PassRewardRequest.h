#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PassTable;

class Packet_PassRewardRequest : public PacketProcessor {

public:
	Packet_PassRewardRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<PassRewardRequest>(nullptr)) {

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				if (pServer->GetMode() == SERVER_MODE::LIVE) {
					if (player->GetIsAuthenticated() == false) {
						pServer->Disconnect(key);
						return;
					}
				}

				const PassRewardRequest* read = flatbuffers::GetRoot<PassRewardRequest>(data);

				TableManager* tableManager = pServer->GetTableManager();
				LobbyManager* lobbyManager = pServer->GetLobbyManager();

				int passIndex = read->pass_index();
				int passType = read->pass_type();
				int passLevel = read->pass_level();
				

				bool result = lobbyManager->GivePassReward(player, passIndex, passType, passLevel);

				PassLevel& passLevelInfo = tableManager->GetPassList()[passIndex].passLevelList[passLevel][passType];

				pPacketSender->SendPassRewardResponsePacket(key, result, passIndex, passType, passLevel, passLevelInfo.Reward_Item_Index, passLevelInfo.Reward_Item_Amount);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_PassRewardRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};