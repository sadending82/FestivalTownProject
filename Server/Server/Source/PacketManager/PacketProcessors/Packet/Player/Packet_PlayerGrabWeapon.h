#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerGrabWeapon : public PacketProcessor {

public:
	Packet_PlayerGrabWeapon(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {

			
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<PlayerGrabWeapon>(nullptr)) {
				const PlayerGrabWeapon* read = flatbuffers::GetRoot<PlayerGrabWeapon>(data);
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

				player->GetBombLock().lock();
				if (player->GetBomb() != nullptr) {
					player->GetBombLock().unlock();
					return;
				}
				player->GetBombLock().unlock();

				player->GetWeaponLock().lock();
				if (player->GetWeapon() != nullptr) {
					player->GetWeaponLock().unlock();
					return;
				}

				int weaponid = read->weapon_id();

				Weapon* weapon = room->GetWeapon(weaponid);
				if (weapon == nullptr) {
					player->GetWeaponLock().unlock();
					return;
				}
				if (weapon->SetIsGrabbed(true) == true) {
					weapon->SetOwenrID(player->GetInGameID());
					player->SetWeapon(weapon);
					std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYER_GRAB_WEAPON, data, size);
					pServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomid);
				}
				else {
					player->SetWeapon(nullptr);
				}
				player->GetWeaponLock().unlock();
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_PlayerGrabWeapon ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};