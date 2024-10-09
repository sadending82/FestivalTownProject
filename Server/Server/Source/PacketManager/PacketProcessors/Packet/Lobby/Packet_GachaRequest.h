#pragma once
#include "../../PacketProcessor.h"
#include <random>

using namespace PacketTable::LobbyTable;

class Packet_GachaRequest : public PacketProcessor {

public:
	Packet_GachaRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) { 
		try {

			mBuilder.Clear();

			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<GachaRequest>(nullptr)) {

				const GachaRequest* read = flatbuffers::GetRoot<GachaRequest>(data);

				int randomBox = read->random_box_index();

				TableManager* tableManager = pServer->GetTableManager();
				int GachaGroup = tableManager->GetRandomBoxList()[randomBox].Gacha_Group;
				std::unordered_map<INDEX, GachaItem>& items = tableManager->GetGachaItemList()[GachaGroup];

				int totalWeight = 0;

				for (auto& item : items) {
					totalWeight += item.second.Gacha_Weight;
				}

				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution<> dis(0, totalWeight);

				int randomWeight = dis(gen);
				int cumulativeWeight = 0;
				int rewardItem = 0;
				int rewardValue = 0;

				for (auto& item : items) {
					cumulativeWeight += item.second.Gacha_Weight;
					if (randomWeight <= cumulativeWeight) {
						rewardItem = item.second.Reward_Item_Index;
						rewardValue = item.second.Reward_Item_Value;
						break;
					}

				}

				// ....
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_GachaRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};