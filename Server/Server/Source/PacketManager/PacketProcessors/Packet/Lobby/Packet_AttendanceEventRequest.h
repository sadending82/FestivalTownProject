#pragma once
#include "../../PacketProcessor.h"
#include <random>

using namespace PacketTable::LobbyTable;

class Packet_AttendanceEventRequest : public PacketProcessor {

public:
	Packet_AttendanceEventRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<AttendanceEventRequest>(nullptr)) {

				const AttendanceEventRequest* read = flatbuffers::GetRoot<AttendanceEventRequest>(data);

				//int eventCode = read->event_code();

				//Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				//int uid = player->GetUID();

				//TableManager* tableManager = pServer->GetTableManager();
				//DB* db = pServer->GetDB();

				//sDayAttendanceInfo dayAttendanceInfo = db->SelectUserAttendanceEventLatest(uid, eventCode);

				//int next_day = dayAttendanceInfo.day_number + 1;
				//bool result = false;

				//if (tableManager->GetEventRewardList()[eventCode].find(next_day) != tableManager->GetEventRewardList()[eventCode].end()) {
				//	result = true;
				//	int reward_code = tableManager->GetEventRewardList()[eventCode][next_day].Reward_Item_Index;
				//	int reward_count = tableManager->GetEventRewardList()[eventCode][next_day].Reward_Item_Value;
				//	// 출석 보상있으면 지급
				//	if (reward_code != 0) {
				//		ItemType itemType = tableManager->GetItemInfos()[reward_code].Item_Type;

				//		if (itemType == ItemType::Money) {
				//			result = db->UpsertUserItemCount(uid, reward_code, reward_count);
				//		}
				//		else {
				//			result = db->InsertUserItem(uid, reward_code, reward_count, (int)itemType);
				//		}
				//	}
				//}

				//pPacketSender->SendAttendanceEventResponsePacket(key, result, eventCode);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_CurrencyAmountRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};