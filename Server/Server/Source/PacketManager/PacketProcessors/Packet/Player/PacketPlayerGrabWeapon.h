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

			if (player->GetWeapon() != nullptr) {
				return;
			}

			int roomid = player->GetRoomID();
			int weaponid = read->weapon_id();

			Room* room = pServer->GetRooms()[roomid];
			room->GetObjectListLock().lock_shared();
			Weapon* weapon = dynamic_cast<Weapon*>(room->GetObjects()[weaponid]);
			if (weapon == nullptr) {
				room->GetObjectListLock().unlock_shared();
				return;
			}
			if (weapon->SetIsGrabbed(true) == true) {
				weapon->SetOwenrID(player->GetInGameID());
				player->SetWeapon(weapon);
				std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYER_GRAB_WEAPON, data, size);
				pServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomid);
			}
			room->GetObjectListLock().unlock_shared();

			//std::cout << "pickup: " << weaponid << std::endl;
		}
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};