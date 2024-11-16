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

				const ChangeCharacterCustomizing* read = flatbuffers::GetRoot<ChangeCharacterCustomizing>(data);

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				int uid = player->GetUID();

				TableManager* tableManager = pServer->GetTableManager();
				DB* db = pServer->GetDB();

				auto customizingInfo = read->customizing_items();

				std::unordered_map<int, UserItem> itemList = db->SelectUserAllItems(uid);

				for (const auto& item : *customizingInfo) {
					if (item->item_code() == 0) {
						continue;
					}

					if (itemList[item->item_uid()].itemCode != item->item_code()) {
						return;
					}
				}

				std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::C2S_CHANGE_CHARACTER_CUSTOMIZING, data, size);
				pServer->SendAllPlayerInRoomExceptSender(send_buffer.data(), send_buffer.size(), key);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_ChangeCharacterCustomizing ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};