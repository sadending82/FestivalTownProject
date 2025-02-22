#pragma once
#include "../../PacketProcessor.h"
#include <random>

using namespace PacketTable::LobbyTable;

class Packet_AttendanceRewardRequest : public PacketProcessor {

public:
	Packet_AttendanceRewardRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (pServer->GetMode() == SERVER_MODE::LIVE && player->GetIsAuthenticated() == false) {
				pServer->Disconnect(key);
				return;
			}

			const AttendanceRewardRequest* read = flatbuffers::GetRoot<AttendanceRewardRequest>(data);

			int eventCode = read->event_code();
			int dayCount = read->day_count();

			int uid = player->GetUID();
			std::unordered_map<int, UserItem>& playerItems = player->GetItems();

			TableManager* tableManager = pServer->GetTableManager();
			DB* db = pServer->GetDB();

			bool isRewarded = db->UpdateUserAttendanceIsRewarded(uid, eventCode, dayCount, true);

			ERROR_CODE result = ERROR_CODE::ER_NONE;

			if (isRewarded == true) {
				return;
			}

			int reward_code = tableManager->GetEventRewardList()[eventCode][dayCount].Reward_Item_Index;
			int reward_amount = tableManager->GetEventRewardList()[eventCode][dayCount].Reward_Item_Value;
			ItemType itemType = tableManager->GetItemInfos()[reward_code].Item_Type;
			// 출석 보상있으면 지급
			if (reward_code != 0) {

				if (itemType == ItemType::Money) {
					result = db->UpsertUserItemCount(uid, reward_code, reward_amount);
				}
				else {
					result = db->InsertUserItem(uid, reward_code, reward_amount, (int)itemType);
				}
			}

			if (ERROR_CODE::ER_NONE != result) {
				std::cout << "AttendanceRewardRequest ERROR " << uid << std::endl;;
			}
			else {
				playerItems[reward_code].owner_UID = uid;
				playerItems[reward_code].itemCode = reward_code;
				playerItems[reward_code].itemType = (int)itemType;
				playerItems[reward_code].count += reward_amount;
			}

			pPacketSender->SendAttendanceRewardResponsePacket(key, eventCode, dayCount, (int)result, reward_code, reward_amount);
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_CurrencyAmountRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};