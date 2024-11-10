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
			if (verifier.VerifyBuffer<GachaRequest>(nullptr)) {

				const CurrencyAmountRequest* read = flatbuffers::GetRoot<CurrencyAmountRequest>(data);

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				int uid = player->GetUID();

				TableManager* tableManager = pServer->GetTableManager();
				DB* db = pServer->GetDB();

				std::vector<int> currencyTypes;
				std::vector<int> currencyAmounts;

				bool result = db->SelectUserAllCurrency(uid, currencyTypes, currencyAmounts);

				pPacketSender->SendCurrencyAmountResponsePacket(key, result, currencyTypes, currencyAmounts);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_CurrencyAmountRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};