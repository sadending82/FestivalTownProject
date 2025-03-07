#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerMove : public PacketProcessor {

public:
	Packet_PlayerMove(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			const PlayerMove* read = flatbuffers::GetRoot<PlayerMove>(data);

			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (player == nullptr && player->GetInGameID() != read->id()) {
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

			player->SetPosition(read->pos()->x(), read->pos()->y(), read->pos()->z());
			player->SetDirection(read->direction()->x(), read->direction()->y(), read->direction()->z());

			std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYER_MOVE, data, size);

			pServer->SendAllPlayerInRoomExceptSender(send_buffer.data(), send_buffer.size(), key, roomID);
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_PlayerMove ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};