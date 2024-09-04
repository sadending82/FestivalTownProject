#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class PacketPlayerGrabWeapon : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();
		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<PlayerGrabWeapon>(nullptr)) {
			const PlayerGrabWeapon* read = flatbuffers::GetRoot<PlayerGrabWeapon>(data);
			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (player == nullptr && player->GetInGameID() != read->id()) {
				return;
			}

			if (player->GetBomb() != nullptr) {
				return;
			}

			int roomid = player->GetRoomID();
			int weaponid = read->weapon_id();

			
		}
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};