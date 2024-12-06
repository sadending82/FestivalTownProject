#pragma once
#include "../../PacketProcessor.h"
#include <random>

using namespace PacketTable::LobbyTable;

class Packet_UserItemsRequest : public PacketProcessor {

public:
	Packet_UserItemsRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<UserItemsRequest>(nullptr)) {

				//const UserItemsRequest* read = flatbuffers::GetRoot<UserItemsRequest>(data);

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				if (pServer->GetMode() == SERVER_MODE::LIVE) {
					if (player->GetIsAuthenticated() == false) {
						pServer->Disconnect(key);
						return;
					}
				}

				int uid = player->GetUID();

				TableManager* tableManager = pServer->GetTableManager();
				DB* db = pServer->GetDB();
				std::pair<ERROR_CODE, std::unordered_map<int, UserItem>> result = db->SelectUserAllItems(uid);
				
				pPacketSender->SendUserItemsResponsePacket(key, result.first, result.second);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_CurrencyAmountRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};