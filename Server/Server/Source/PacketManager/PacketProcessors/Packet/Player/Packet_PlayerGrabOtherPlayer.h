#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerGrabOtherPlayer : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();
		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<PlayerGrabOtherPlayer>(nullptr)) {
			const PlayerGrabOtherPlayer* read = flatbuffers::GetRoot<PlayerGrabOtherPlayer >(data);

			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (player == nullptr && player->GetInGameID() != read->id()) {
				return;
			}
			int playerID = read->id();
			int targetID = read->target_id();

			int roomID = player->GetRoomID();
			Room* room = pServer->GetRooms()[roomID];
			if (room == nullptr && (room->GetState() != eRoomState::RS_INGAME)) {
				return;
			}
			room->GetPlayerListLock().lock_shared();
			Player* target = room->GetPlayerList()[read->target_id()];

			if (target->SetIsGrabbed(true) == false) {
				room->GetPlayerListLock().unlock_shared();
				return;
			}
			player->SetAttachedPlayerID(targetID);
			target->SetAttachedPlayerID(playerID);


			player->SetPosition(read->pos()->x(), read->pos()->y(), read->pos()->z());
			player->SetDirection(read->direction()->x(), read->direction()->y(), read->direction()->z());

			room->GetPlayerListLock().unlock_shared();
			std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYER_GRAB_OTHER_PLAYER, data, size);

			pServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);

		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};