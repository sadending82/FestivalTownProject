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
				int uid = player->GetUID();

				int randomBox = read->random_box_index();
				int mileageIndex = 100003;

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
					int currPayItemAmount = db->SelectUserItemCount(uid, pay_item[i]);

					if (pay_Price[i] <= currPayItemAmount) {
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

						int itemType = (int)tableManager->GetItemInfos()[rewardItem].Item_Type;

						// 재화나 티켓인 경우
						if (itemType == (int)ItemType::Money) {
							db->UpsertUserItemCount(uid, rewardItem, rewardValue);
							db->UpsertUserItemCount(uid, pay_item[i], -pay_Price[i]);
							gacha_result = true;
							break;
						}
						else {
							int currItemCount = db->SelectUserItemCount(uid, rewardItem);
							// 새로 얻은 경우
							if (currItemCount == 0) {
								bool insert_result = db->InsertUserItem(uid, rewardItem, rewardValue, itemType);
								if (insert_result == false) {
									gacha_result = false;
									break;
								}
							}
							// 이미 있는 경우
							else {
								// 마일리지 지급
								int itemGrade = (int)tableManager->GetItemInfos()[rewardItem].Item_Grade;
								int mileage = tableManager->GetGachaAcquiredMileages()[itemGrade];
								bool update_result = db->UpsertUserItemCount(uid, mileageIndex, mileage);
								if (update_result == false) {
									gacha_result = false;
									break;
								}

								rewardItem = mileageIndex;
								rewardValue = mileage;
							}
						}

						db->UpsertUserItemCount(uid, pay_item[i], -pay_Price[i]);
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