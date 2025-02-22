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

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				if (pServer->GetMode() == SERVER_MODE::LIVE) {
					if (player->GetIsAuthenticated() == false) {
						pServer->Disconnect(key);
						return;
					}
				}

				//const AttendanceEventRequest* read = flatbuffers::GetRoot<AttendanceEventRequest>(data);

				//int eventCode = read->event_code();

				//int uid = player->GetUID();

				//TableManager* tableManager = pServer->GetTableManager();
				//DB* db = pServer->GetDB();

				//sDayAttendanceInfo dayAttendanceInfo = db->SelectUserAttendanceEventLatest(uid, eventCode);

				//int next_day = dayAttendanceInfo.day_number + 1;

				//std::time_t nowTime = std::time(nullptr);
				//std::tm tNowTime = {};
				//localtime_s(&tNowTime, &nowTime);

				//if (tNowTime.tm_year == dayAttendanceInfo.attendance_date.tm_year
				//	&& tNowTime.tm_mon == dayAttendanceInfo.attendance_date.tm_mon
				//	&& tNowTime.tm_mday == dayAttendanceInfo.attendance_date.tm_mday) {
				//	return;
				//}

				//ERROR_CODE result = ERROR_CODE::ER_DB_NO_DATA;
				///*int reward_code = 0;
				//int reward_amount = 0;*/

				//if (tableManager->GetEventRewardList()[eventCode].count(next_day) != 0) {
				//	result = db->InsertUserAttendance(uid, eventCode, next_day);

				//	//if (result == ERROR_CODE::ER_NONE) {
				//	//	reward_code = tableManager->GetEventRewardList()[eventCode][next_day].Reward_Item_Index;
				//	//	reward_amount = tableManager->GetEventRewardList()[eventCode][next_day].Reward_Item_Value;
				//	//	// 출석 보상있으면 지급
				//	//	if (reward_code != 0) {
				//	//		ItemType itemType = tableManager->GetItemInfos()[reward_code].Item_Type;

				//	//		if (itemType == ItemType::Money) {
				//	//			result = db->UpsertUserItemCount(uid, reward_code, reward_amount);
				//	//		}
				//	//		else {
				//	//			result = db->InsertUserItem(uid, reward_code, reward_amount, (int)itemType);
				//	//		}
				//	//	}
				//	//}
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