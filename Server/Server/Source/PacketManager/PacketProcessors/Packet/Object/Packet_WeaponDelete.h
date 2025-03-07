#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::ObjectTable;

class Packet_WeaponDelete : public PacketProcessor {

public:
	Packet_WeaponDelete(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			const WeaponDelete* read = flatbuffers::GetRoot<WeaponDelete>(data);

			int roomID = dynamic_cast<Player*>(pServer->GetSessions()[key])->GetRoomID();
			if (roomID == INVALIDKEY) {
				return;
			}
			int weaponid = read->id();
			if (weaponid <= INVALIDKEY) {
				return;
			}
			if (roomID == INVALIDKEY) {
				return;
			}
			Room* room = pServer->GetRooms().at(roomID);
			if (room == nullptr) {
				return;
			}
			if (room->GetState() == eRoomState::RS_FREE) {
				return;
			}

			if (room->GetWeapon(weaponid) != nullptr) {
				std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_WEAPON_DELETE, data, size);
				pServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
				room->DeleteWeapon(weaponid);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_WeaponDelete ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};