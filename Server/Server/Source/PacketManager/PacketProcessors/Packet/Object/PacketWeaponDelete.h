#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::ObjectTable;

class PacketWeaponDelete : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<WeaponDelete>(nullptr)) {
			const WeaponDelete* read = flatbuffers::GetRoot<WeaponDelete>(data);

			int roomid = dynamic_cast<Player*>(pServer->GetSessions()[key])->GetRoomID();
			int weaponid = read->id();
			if (weaponid <= INVALIDKEY) {
				return;
			}
			Room* room = pServer->GetRooms()[roomid];
			if (room == nullptr) {
				return;
			}
			if (room->GetState() == eRoomState::RS_FREE) {
				return;
			}

			std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_WEAPON_DELETE, data, size);
			pServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomid);
			room->DeleteWeapon(weaponid);
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};