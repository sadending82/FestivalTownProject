#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::ObjectTable;

class Packet_PlayerDropWeapon : public PacketProcessor {

public:
	Packet_PlayerDropWeapon(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) { 
		try {
			const WeaponPosition* read = flatbuffers::GetRoot<WeaponPosition>(data);

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


			player->GetWeaponLock().lock();
			Weapon* weapon = player->GetWeapon();
			if (weapon == nullptr) {
				std::cout << "무기 없어서 못버림\n";
				player->GetWeaponLock().unlock();
				return;
			}

			int weaponid = read->id();

			weapon->SetIsGrabbed(false);
			weapon->SetOwenrID(INVALIDKEY);
			weapon->SetPosition(Vector3f(read->pos()->x(), read->pos()->y(), read->pos()->z()));
			player->SetWeapon(nullptr);

			player->GetWeaponLock().unlock();

			std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYER_DROP_WEAPON, data, size);
			pServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);

			//std::cout << "drop: " << weaponid << std::endl;
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_PlayerDropWeapon ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};