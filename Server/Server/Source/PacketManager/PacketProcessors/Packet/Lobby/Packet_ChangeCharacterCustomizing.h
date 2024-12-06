#pragma once
#include "../../PacketProcessor.h"
#include <random>

using namespace PacketTable::LobbyTable;

class Packet_ChangeCharacterCustomizing : public PacketProcessor {

public:
	Packet_ChangeCharacterCustomizing(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<ChangeCharacterCustomizing>(nullptr)) {

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				if (pServer->GetMode() == SERVER_MODE::LIVE) {
					if (player->GetIsAuthenticated() == false) {
						pServer->Disconnect(key);
						return;
					}
				}

				const ChangeCharacterCustomizing* read = flatbuffers::GetRoot<ChangeCharacterCustomizing>(data);

				int uid = player->GetUID();

				TableManager* tableManager = pServer->GetTableManager();
				DB* db = pServer->GetDB();

				auto customizingInfo = read->customizing_items();

				std::pair<ERROR_CODE, std::unordered_map<int, UserItem>> result = db->SelectUserAllItems(uid);

				std::unordered_map<int, UserItem>& itemList = result.second;
				sCharacterCustomizing characterCustomizing;

				for (const auto& item : *customizingInfo) {
					if (item->item_code() == 0) {
						continue;
					}

					if (itemList[item->item_uid()].itemCode != item->item_code()) {
						return;
					}

					EquippedItem itemInfo;
					itemInfo.item_UID = item->item_uid();
					itemInfo.itemCode = item->item_code();
					itemInfo.itemType = item->type();

					characterCustomizing.SetItem((CustomizingItemType)item->type(), itemInfo);
				}

				if (db->UpdateCharacterCustomizing(uid, characterCustomizing) == ERROR_CODE::ER_NONE) {
					player->SetCharacterCustomizing(characterCustomizing);
				}
				else {
					return;
				}

				std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_CHANGE_CHARACTER_CUSTOMIZING, data, size);
				player->DoSend(send_buffer.data(), send_buffer.size());
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_ChangeCharacterCustomizing ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};