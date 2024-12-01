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

				const CurrencyAmountRequest* read = flatbuffers::GetRoot<CurrencyAmountRequest>(data);

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				int uid = player->GetUID();

				TableManager* tableManager = pServer->GetTableManager();
				DB* db = pServer->GetDB();

				bool result = false;
				std::vector<UserItem> currencyList = db->SelectUserAllCurrency(uid);

				if (currencyList.empty() == false) {
					result = true;
				}

				pPacketSender->SendCurrencyAmountResponsePacket(key, result, currencyList);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_CurrencyAmountRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};