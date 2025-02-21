#pragma once
#include "../../PacketProcessor.h"
#include <random>

using namespace PacketTable::LobbyTable;

class Packet_EventRewardRequest : public PacketProcessor {

public:
	Packet_EventRewardRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			
			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (pServer->GetMode() == SERVER_MODE::LIVE && player->GetIsAuthenticated() == false) {
				pServer->Disconnect(key);
				return;
			}

			const EventRewardRequest* read = flatbuffers::GetRoot<EventRewardRequest>(data);

			int eventCode = read->event_code();
			int uid = player->GetUID();

			TableManager* tableManager = pServer->GetTableManager();
			DB* db = pServer->GetDB();

			ERROR_CODE result = db->UpdateUserEventReward_IsRewarded(uid, eventCode);

			if (result != ERROR_CODE::ER_NONE) {
				std::cout << "EventRewardRequest ERROR " << uid << std::endl;;
				return;
			}

			int reward_code = tableManager->GetEventRewardList()[eventCode][1].Reward_Item_Index;
			int reward_amount = tableManager->GetEventRewardList()[eventCode][1].Reward_Item_Value;
			if (reward_code != 0) {
				ItemType itemType = tableManager->GetItemInfos()[reward_code].Item_Type;

				if (itemType == ItemType::Money) {
					result = db->UpsertUserItemCount(uid, reward_code, reward_amount);
				}
				else {
					result = db->InsertUserItem(uid, reward_code, reward_amount, (int)itemType);
				}
			}

			pPacketSender->SendEventRewardResponsePacket(key, eventCode, (int)result, reward_code, reward_amount);
			
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_CurrencyAmountRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};