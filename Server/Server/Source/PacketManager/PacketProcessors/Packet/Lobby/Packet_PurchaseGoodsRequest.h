#pragma once
#include "../../PacketProcessor.h"
#include <random>

using namespace PacketTable::ShopTable;

class Packet_PurchaseGoodsRequest : public PacketProcessor {

public:
	Packet_PurchaseGoodsRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<PurchaseGoodsRequest>(nullptr)) {

				const PurchaseGoodsRequest* read = flatbuffers::GetRoot<PurchaseGoodsRequest>(data);

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				if (pServer->GetMode() == SERVER_MODE::LIVE && player->GetIsAuthenticated() == false) {
					pServer->Disconnect(key);
					return;
				}

				int uid = player->GetUID();

				int goods_index = read->index();

				TableManager* tableManager = pServer->GetTableManager();
				DB* db = pServer->GetDB();

				Shop_Goods& goodsInfo = pServer->GetTableManager()->GetShopGoodsList()[goods_index];
				int item_index = goodsInfo.Item;
				int item_amount = goodsInfo.Item_Amount;

				ItemType item_type = pServer->GetTableManager()->GetItemInfos()[item_index].Item_Type;

				bool result = false;

				const ERROR_CODE payResult = db->UpdateUserItemCount(uid, goodsInfo.Currency_ID, -goodsInfo.Price);

				if (payResult == ERROR_CODE::ER_DB_ERROR || payResult == ERROR_CODE::ER_DB_NO_DATA) {
					result = false;
				}

				const ERROR_CODE insertReceiptResult = db->InsertReceipt(uid, goods_index);

				switch (item_type) {
				case ItemType::Money: {
					const ERROR_CODE upsertResult = db->UpsertUserItemCount(uid, item_index, item_amount);
					if (upsertResult == ERROR_CODE::ER_DB_ERROR) {
						db->UpdateUserItemCount(uid, goodsInfo.Currency_ID, goodsInfo.Price);
					}
				}break;
				case ItemType::Skin:
				case ItemType::Emotion:
				case ItemType::Accessory_Back:
				case ItemType::Accessory_Face:
				case ItemType::Accessory_Head: {
					int currItemCount = db->SelectUserItemCount(uid, item_index);
					// 보유한 아이템이 아닌 경우
					if (currItemCount == 0) {
						const ERROR_CODE insertResult = db->InsertUserItem(uid, item_index, item_amount, (int)item_type);
						if (insertResult == ERROR_CODE::ER_DB_ERROR) {
							db->UpdateUserItemCount(uid, goodsInfo.Currency_ID, goodsInfo.Price);
						}
						result = true;
					}
				}break;
				default: {

				}break;
				}

				int curr_currency = db->SelectUserItemCount(uid, goodsInfo.Currency_ID);

				pPacketSender->SendPurchaseGoodsResponsePacket(key, result, item_index, item_amount, goodsInfo.Currency_ID, curr_currency);

			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_CurrencyAmountRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};