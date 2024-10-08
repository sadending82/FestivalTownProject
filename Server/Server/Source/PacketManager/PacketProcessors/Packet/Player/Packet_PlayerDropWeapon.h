#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::ObjectTable;

class Packet_PlayerDropWeapon : public PacketProcessor {

public:
	Packet_PlayerDropWeapon(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) { 
		try {

			mBuilder.Clear();
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<WeaponPosition>(nullptr)) {
				const WeaponPosition* read = flatbuffers::GetRoot<WeaponPosition>(data);

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				if (player == nullptr && player->GetInGameID() != read->id()) {
					return;
				}
				player->GetWeaponLock().lock();
				Weapon* weapon = player->GetWeapon();
				if (weapon == nullptr) {
					player->GetWeaponLock().unlock();
					return;
				}

				int roomid = player->GetRoomID();
				int weaponid = read->id();
				Room* room = pServer->GetRooms().at(roomid);

				weapon->SetIsGrabbed(false);
				weapon->SetOwenrID(INVALIDKEY);
				weapon->SetPosition(Vector3f(read->pos()->x(), read->pos()->y(), read->pos()->z()));
				player->SetWeapon(nullptr);

				player->GetWeaponLock().unlock();

				std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYER_DROP_WEAPON, data, size);
				pServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomid);

				//std::cout << "drop: " << weaponid << std::endl;
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};