#pragma once
#include "../../PacketProcessor.h"
#include <random>

using namespace PacketTable::LobbyTable;

class Packet_AttendanceRewardRequest : public PacketProcessor {

public:
	Packet_AttendanceRewardRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<AttendanceRewardRequest>(nullptr)) {

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				if (pServer->GetMode() == SERVER_MODE::LIVE) {
					if (player->GetIsAuthenticated() == false) {
						pServer->Disconnect(key);
						return;
					}
				}

				const AttendanceRewardRequest* read = flatbuffers::GetRoot<AttendanceRewardRequest>(data);

				int eventCode = read->event_code();
				int dayCount = read->day_count();

				int uid = player->GetUID();

				TableManager* tableManager = pServer->GetTableManager();
				DB* db = pServer->GetDB();

				bool isRewarded = db->UpdateUserAttendanceIsRewarded(uid, eventCode, dayCount, true);

				ERROR_CODE result = ERROR_CODE::ER_NONE;

				if (isRewarded == true) {
					return;
				}

				int reward_code = tableManager->GetEventRewardList()[eventCode][dayCount].Reward_Item_Index;
				int reward_amount = tableManager->GetEventRewardList()[eventCode][dayCount].Reward_Item_Value;
				// 출석 보상있으면 지급
				if (reward_code != 0) {
					ItemType itemType = tableManager->GetItemInfos()[reward_code].Item_Type;

					if (itemType == ItemType::Money) {
						result = db->UpsertUserItemCount(uid, reward_code, reward_amount);
					}
					else {
						result = db->InsertUserItem(uid, reward_code, reward_amount, (int)itemType);
					}
				}

				pPacketSender->SendAttendanceRewardResponsePacket(key, eventCode, dayCount, (int)result, reward_code, reward_amount);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_CurrencyAmountRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};