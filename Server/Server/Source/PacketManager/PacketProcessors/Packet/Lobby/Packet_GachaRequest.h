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

				TableManager* tableManager = pServer->GetTableManager();
				DB* db = pServer->GetDB();
				LobbyManager* lobbyManager = pServer->GetLobbyManager();

				int GachaGroup = tableManager->GetRandomBoxList()[randomBox].Gacha_Group;
				std::unordered_map<INDEX, GachaItem>& items = tableManager->GetGachaItemList()[GachaGroup];

				int pay_item[2];
				int pay_Price[2];

				pay_item[0] = tableManager->GetRandomBoxList()[randomBox].Pay_Item_Index1;
				pay_Price[0] = tableManager->GetRandomBoxList()[randomBox].Pay_Item_Value1;

				pay_item[1] = tableManager->GetRandomBoxList()[randomBox].Pay_Item_Index2;
				pay_Price[1] = tableManager->GetRandomBoxList()[randomBox].Pay_Item_Value2;
				
				int gacha_result = false;
				
				GachaItem rewardItem;

				// ���� ���� ���� ��ȭ ����
				int usedItemCode = 0;
				int usedItemAmount = 0;
				int usedItemRemaining = 0;

				for (int i = 1; i >= 0; i--) {

					if (pay_item[i] == NONE_ITEM_INDEX) {
						continue;
					}

					int currPayItemAmount = db->SelectUserItemCount(uid, pay_item[i]);

					if (pay_Price[i] <= currPayItemAmount) {
						rewardItem = lobbyManager->RollGacha(items);
						
						gacha_result = lobbyManager->GiveGachaItemToUser(uid, pay_item[i], pay_Price[i], rewardItem);

						usedItemCode = pay_item[i];
						usedItemAmount = pay_Price[i];
						usedItemRemaining = currPayItemAmount - usedItemAmount;
						break;
					}
				}

				pPacketSender->SendGachaResponsePacket(key, gacha_result, rewardItem.Reward_Item_Index, rewardItem.Reward_Item_Value, usedItemCode, usedItemAmount, usedItemRemaining);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_GachaRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};