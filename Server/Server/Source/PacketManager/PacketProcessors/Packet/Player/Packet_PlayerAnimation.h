#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerAnimation : public PacketProcessor {

public:
	Packet_PlayerAnimation(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) { 
		try {
			const PlayerAnimation* read = flatbuffers::GetRoot<PlayerAnimation>(data);

			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (player == nullptr && player->GetInGameID() != read->id()) {
				return;
			}
			int roomID = player->GetRoomID();
			if (roomID == INVALIDKEY) {
				return;
			}
			Room* room = pServer->GetRooms()[roomID];
			if (room == nullptr && (room->GetState() != eRoomState::RS_INGAME)) {
				return;
			}

			std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYER_ANIMATION, data, size);

			pServer->SendAllPlayerInRoomExceptSender(send_buffer.data(), send_buffer.size(), key, roomID);
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_PlayerAnimation ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};