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
			std::unordered_map<int, UserItem>& playerItems = player->GetItems();
			int goods_index = read->index();

			TableManager* tableManager = pServer->GetTableManager();
			DB* db = pServer->GetDB();

			Shop_Goods& goodsInfo = tableManager->GetShopGoodsList()[goods_index];
			int item_index = goodsInfo.Item;
			int item_amount = goodsInfo.Item_Amount;
			ItemType item_type = pServer->GetTableManager()->GetItemInfos()[item_index].Item_Type;

			bool result = false;
			int curr_currency = playerItems[goodsInfo.Currency_ID].count;

			// 재화량 확인
			if (curr_currency < goodsInfo.Price) {
				pPacketSender->SendPurchaseGoodsResponsePacket(key, false, goods_index, goodsInfo.Currency_ID, curr_currency);
				return;
			}

			// 재화 소모
			const ERROR_CODE payResult = db->UpdateUserItemCount(uid, goodsInfo.Currency_ID, -goodsInfo.Price);
			if (payResult == ERROR_CODE::ER_DB_ERROR || payResult == ERROR_CODE::ER_DB_NO_DATA) {
				pPacketSender->SendPurchaseGoodsResponsePacket(key, false, goods_index, goodsInfo.Currency_ID, curr_currency);
				return;
			}
			playerItems[goodsInfo.Currency_ID].count -= goodsInfo.Price;
			curr_currency = playerItems[goodsInfo.Currency_ID].count;

			const ERROR_CODE insertReceiptResult = db->InsertReceipt(uid, goods_index);

			// 아이템 지급
			switch (item_type) {
			case ItemType::Money: {
				const ERROR_CODE upsertResult = db->UpsertUserItemCount(uid, item_index, item_amount);
				if (upsertResult == ERROR_CODE::ER_DB_ERROR) {
					result = false;
				}
				else {
					result = true;
				}
			}break;
			case ItemType::Skin:
			case ItemType::Emotion:
			case ItemType::Accessory_Back:
			case ItemType::Accessory_Face:
			case ItemType::Accessory_Head: {
				int currItemCount = player->GetItems().count(item_index);
				// 보유한 아이템이 아닌 경우
				if (currItemCount == 0) {
					const ERROR_CODE insertResult = db->InsertUserItem(uid, item_index, item_amount, (int)item_type);
					if (insertResult == ERROR_CODE::ER_DB_ERROR) {
						result = false;
					}
					else {
						result = true;
					}
				}
			}break;
			default: {

			}break;
			}

			// 구매 성공
			if (result == true) {
				playerItems[item_index].owner_UID = uid;
				playerItems[item_index].itemCode = item_index;
				playerItems[item_index].itemType = (int)item_type;
				playerItems[item_index].count = item_amount;
			}
			// 구매 실패
			else {
				db->UpdateUserItemCount(uid, goodsInfo.Currency_ID, goodsInfo.Price);
				playerItems[goodsInfo.Currency_ID].count += goodsInfo.Price;
				curr_currency = playerItems[goodsInfo.Currency_ID].count;
			}

			pPacketSender->SendPurchaseGoodsResponsePacket(key, result, goods_index, goodsInfo.Currency_ID, curr_currency);
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_CurrencyAmountRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};