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

				// 임시 / 수정해야함
				std::unordered_map<int, UserItem> currencyList;
				currencyList[100001] = player->GetItems()[100001];
				currencyList[100002] = player->GetItems()[100002];
				currencyList[100003] = player->GetItems()[100003];

				pPacketSender->SendCurrencyAmountResponsePacket(key, true, currencyList);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_CurrencyAmountRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};