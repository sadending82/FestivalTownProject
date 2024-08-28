#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class PacketPlayerGrabBomb : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();
		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<PlayerGrabBomb>(nullptr)) {
			const PlayerGrabBomb* read = flatbuffers::GetRoot<PlayerGrabBomb>(data);
			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (player == nullptr && player->GetInGameID() != read->id()) {
				return;
			}
			int roomid = player->GetRoomID();
			int bombid = read->bomb_id();

			Room* room = pServer->GetRooms()[roomid];
			room->GetObjectListLock().lock_shared();
			Bomb* bomb = dynamic_cast<Bomb*>(room->GetObjects()[bombid]);
			if (bomb == nullptr) {
				room->GetObjectListLock().unlock_shared();
				return;
			}
			if (bomb->SetIsGrabbed(true) == true) {
				std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYERGRABBOMB, data, size);
				pServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomid);
			}
			room->GetObjectListLock().unlock_shared();
		}
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};