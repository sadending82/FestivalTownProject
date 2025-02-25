#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::CheatTable;

class Packet_GetCurrency : public PacketProcessor {

public:
	Packet_GetCurrency(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		if (pServer->GetMode() != SERVER_MODE::TEST) {
			return;
		}
		const GetCurrency* read = flatbuffers::GetRoot<GetCurrency>(data);
		Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
		if (player == nullptr) {
			return;
		}


		int uid = player->GetUID();
		DB* db = pServer->GetDB();

		db->UpsertUserItemCount(uid, read->item_code(), read->count());

		std::pair<ERROR_CODE, std::vector<UserItem>> result = db->SelectUserAllCurrency(uid);
	}

private:
};