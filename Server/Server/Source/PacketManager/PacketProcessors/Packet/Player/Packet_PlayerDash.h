#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerDash : public PacketProcessor {

public:
	Packet_PlayerDash(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			const PlayerDash* read = flatbuffers::GetRoot<PlayerDash>(data);

			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);

			auto playerInfo = read->player_info();

			if (player == nullptr && player->GetInGameID() != playerInfo->id()) {
				return;
			}
			int roomID = player->GetRoomID();
			if (roomID == INVALIDKEY) {
				return;
			}
			Room* room = pServer->GetRooms().at(roomID);
			if (room == nullptr && (room->GetState() != eRoomState::RS_INGAME)) {
				return;
			}

			if (player->GetIsGrabbed() == true) {
				return;
			}

			player->SetPosition(playerInfo->pos()->x(), playerInfo->pos()->y(), playerInfo->pos()->z());
			player->SetDirection(playerInfo->direction()->x(), playerInfo->direction()->y(), playerInfo->direction()->z());

			std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYER_DASH, data, size);

			pServer->SendAllPlayerInRoomExceptSender(send_buffer.data(), send_buffer.size(), key, roomID);
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_PlayerAnimation ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};