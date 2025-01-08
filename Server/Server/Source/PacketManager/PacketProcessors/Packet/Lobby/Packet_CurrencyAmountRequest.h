#pragma once
#include "../../PacketProcessor.h"
#include <random>

using namespace PacketTable::LobbyTable;

class Packet_CurrencyAmountRequest : public PacketProcessor {

public:
	Packet_CurrencyAmountRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<CurrencyAmountRequest>(nullptr)) {

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				if (pServer->GetMode() == SERVER_MODE::LIVE) {
					if (player->GetIsAuthenticated() == false) {
						pServer->Disconnect(key);
						return;
					}
				}

				const CurrencyAmountRequest* read = flatbuffers::GetRoot<CurrencyAmountRequest>(data);

				int uid = player->GetUID();

				TableManager* tableManager = pServer->GetTableManager();
				DB* db = pServer->GetDB();

				std::pair<ERROR_CODE, std::vector<UserItem>> result = db->SelectUserAllCurrency(uid);
				
				if (ERROR_CODE::ER_NONE != result.first) {
					std::cout << "SelectUserAllCurrency ERROR " << uid << std::endl;;
				}

				pPacketSender->SendCurrencyAmountResponsePacket(key, result.first, result.second);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_CurrencyAmountRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};