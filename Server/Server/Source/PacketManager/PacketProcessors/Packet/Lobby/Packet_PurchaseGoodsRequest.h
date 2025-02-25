#pragma once
#include "../../PacketProcessor.h"
#include <random>

using namespace PacketTable::ShopTable;

class Packet_PurchaseGoodsRequest : public PacketProcessor {

public:
	Packet_PurchaseGoodsRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			const PurchaseGoodsRequest* read = flatbuffers::GetRoot<PurchaseGoodsRequest>(data);

			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (pServer->GetMode() == SERVER_MODE::LIVE && player->GetIsAuthenticated() == false) {
				pServer->Disconnect(key);
				return;
			}

			int uid = player->GetUID();
			int goods_index = read->index();

			TableManager* tableManager = pServer->GetTableManager();
			LobbyManager* lobbyManager = pServer->GetLobbyManager();

			Shop_Goods& goodsInfo = tableManager->GetShopGoodsList()[goods_index];
			bool result = false;
			int curr_currency = player->GetItems()[goodsInfo.Currency_ID].count;

			switch (goodsInfo.Category_Index) {
			case 403: {
				result = lobbyManager->PurchasePass(player, goodsInfo, goods_index);
				pPacketSender->SendUserPassStatePacket(key, player->GetPassInfo()[goodsInfo.Item]);
			}break;
			default: {
				result = lobbyManager->PurchaseShopGoods(player, goodsInfo, goods_index);
			}break;
			}


			pPacketSender->SendPurchaseGoodsResponsePacket(key, result, goods_index, goodsInfo.Currency_ID, curr_currency);
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_CurrencyAmountRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};