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


				TableManager* tableManager = pServer->GetTableManager();
				int uid = player->GetUID();

				std::unordered_map<int, UserItem> playerCurrencyList;

				std::unordered_map<int, ItemTable> currencyList = tableManager->GetCurrencyList();

				for (auto& currency : currencyList) {
					if (currency.first == 0) {
						continue;
					}
					const int index = currency.first;
					playerCurrencyList[index] = player->GetItems()[index];
				}

				pPacketSender->SendCurrencyAmountResponsePacket(key, true, playerCurrencyList);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_CurrencyAmountRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};