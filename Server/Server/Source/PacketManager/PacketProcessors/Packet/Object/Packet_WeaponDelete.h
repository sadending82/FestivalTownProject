#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::ObjectTable;

class Packet_WeaponDelete : public PacketProcessor {

public:
	Packet_WeaponDelete(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {

		try {
			

			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<WeaponDelete>(nullptr)) {
				const WeaponDelete* read = flatbuffers::GetRoot<WeaponDelete>(data);

				int roomid = dynamic_cast<Player*>(pServer->GetSessions()[key])->GetRoomID();
				if (roomid == INVALIDKEY) {
					return;
				}
				int weaponid = read->id();
				if (weaponid <= INVALIDKEY) {
					return;
				}
				if (roomid == INVALIDKEY) {
					return;
				}
				Room* room = pServer->GetRooms().at(roomid);
				if (room == nullptr) {
					return;
				}
				if (room->GetState() == eRoomState::RS_FREE) {
					return;
				}

				room->GetWeaponListLock().lock();
				if (room->GetWeaponList()[weaponid] != nullptr) {
					std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_WEAPON_DELETE, data, size);
					pServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomid);
					room->DeleteWeapon(weaponid);
				}
				room->GetWeaponListLock().unlock();
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_WeaponDelete ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};