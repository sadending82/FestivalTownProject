#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerThrowOtherPlayer : public PacketProcessor {

public:
	Packet_PlayerThrowOtherPlayer(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<PlayerThrowOtherPlayer>(nullptr)) {
				const PlayerThrowOtherPlayer* read = flatbuffers::GetRoot<PlayerThrowOtherPlayer>(data);

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				if (player == nullptr && player->GetInGameID() != read->id()) {
					return;
				}

				int playerID = read->id();
				int targetID = read->targetID();

				int roomID = player->GetRoomID();
				if (roomID == INVALIDKEY) {
					return;
				}
				Room* room = pServer->GetRooms().at(roomID);
				if (room == nullptr && (room->GetState() != eRoomState::RS_INGAME)) {
					return;
				}

				int target_sessionID = room->GetPlayerList()[read->targetID()].load();
				if (target_sessionID == INVALIDKEY) {
					return;
				}

				Player* target = dynamic_cast<Player*>(pServer->GetSessions()[target_sessionID]);

				if (target->SetIsGrabbed(false) == true) {
					player->SetAttachedPlayerID(INVALIDKEY);
					target->SetAttachedPlayerID(INVALIDKEY);

					player->SetPosition(read->pos()->x(), read->pos()->y(), read->pos()->z());
					player->SetDirection(read->direction()->x(), read->direction()->y(), read->direction()->z());

					std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYER_THROW_OTHER_PLAYER, data, size);

					pServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
				}
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_PlayerThrowOtherPlayer ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};