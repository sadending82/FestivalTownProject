#pragma once
#include "../../PacketProcessor.h"
#include <random>

using namespace PacketTable::LobbyTable;

class Packet_GachaRequest : public PacketProcessor {

public:
	Packet_GachaRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) { 
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<GachaRequest>(nullptr)) {

				const GachaRequest* read = flatbuffers::GetRoot<GachaRequest>(data);

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);

				int randomBox = read->random_box_index();

				TableManager* tableManager = pServer->GetTableManager();
				DB* db = pServer->GetDB();

				int GachaGroup = tableManager->GetRandomBoxList()[randomBox].Gacha_Group;
				std::unordered_map<INDEX, GachaItem>& items = tableManager->GetGachaItemList()[GachaGroup];

				int pay_item[2];
				int pay_Price[2];

				pay_item[0] = tableManager->GetRandomBoxList()[randomBox].Pay_Item_Index1;
				pay_Price[0] = tableManager->GetRandomBoxList()[randomBox].Pay_Item_Value1;

				pay_item[1] = tableManager->GetRandomBoxList()[randomBox].Pay_Item_Index2;
				pay_Price[1] = tableManager->GetRandomBoxList()[randomBox].Pay_Item_Value2;
				
				int gacha_result = false;
				int rewardItem = 0;
				int rewardValue = 0;

				for (int i = 1; i >= 0; i--) {
					int currAmount = db->SelectUserItemCount(player->GetUID(), pay_item[i]);

					COUT << currAmount << ENDL;

					if (pay_Price[i] <= currAmount) {
						int totalWeight = 0;

						for (auto& item : items) {
							totalWeight += item.second.Gacha_Weight;
						}

						std::random_device rd;
						std::mt19937 gen(rd());
						std::uniform_int_distribution<> dis(0, totalWeight);

						int randomWeight = dis(gen);
						int cumulativeWeight = 0;
						rewardItem = 0;
						rewardValue = 0;

						for (auto& item : items) {
							cumulativeWeight += item.second.Gacha_Weight;
							if (randomWeight <= cumulativeWeight) {
								rewardItem = item.second.Reward_Item_Index;
								rewardValue = item.second.Reward_Item_Value;
								break;
							}

						}

						db->UpdateUserItemCount(player->GetUID(), pay_item[i], -pay_Price[i]);
						gacha_result = true;
						break;
					}
				}

				// ....

				pPacketSender->SendGachaResponsePacket(key, gacha_result, rewardItem, rewardValue, 0, 0, 0);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_GachaRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};