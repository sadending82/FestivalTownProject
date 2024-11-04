#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerStop : public PacketProcessor {

public:
	Packet_PlayerStop(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<PlayerStop>(nullptr)) {


				const PlayerStop* read = flatbuffers::GetRoot<PlayerStop>(data);

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				if (player == nullptr && player->GetInGameID() != read->id()) {
					return;
				}

				int roomid = player->GetRoomID();
				if (roomid == INVALIDKEY) {
					return;
				}
				Room* room = pServer->GetRooms().at(roomid);
				if (room == nullptr && (room->GetState() != eRoomState::RS_INGAME)) {
					return;
				}

				if (player->GetIsGrabbed() == true) {
					return;
				}

				player->SetPosition(read->pos()->x(), read->pos()->y(), read->pos()->z());
				player->SetDirection(read->direction()->x(), read->direction()->y(), read->direction()->z());

				std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYER_STOP, data, size);
				pServer->SendAllPlayerInRoomExceptSender(send_buffer.data(), send_buffer.size(), key);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_PlayerStop ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};