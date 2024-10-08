#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerAnimation : public PacketProcessor {

public:
	Packet_PlayerAnimation(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) { 
		try {

			mBuilder.Clear();

			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<PlayerAnimation>(nullptr)) {

				const PlayerAnimation* read = flatbuffers::GetRoot<PlayerAnimation>(data);

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				if (player == nullptr && player->GetInGameID() != read->id()) {
					return;
				}

				Room* room = pServer->GetRooms()[player->GetRoomID()];
				if (room == nullptr && (room->GetState() != eRoomState::RS_INGAME)) {
					return;
				}

				std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYER_ANIMATION, data, size);

				pServer->SendAllPlayerInRoomExceptSender(send_buffer.data(), send_buffer.size(), key);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};