#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::ObjectTable;

class Packet_BombExplosion : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<BombExplosion>(nullptr)) {
			const BombExplosion* read = flatbuffers::GetRoot<BombExplosion>(data);

			int roomid = dynamic_cast<Player*>(pServer->GetSessions()[key])->GetRoomID();
			int bombid = read->id();
			if (bombid <= INVALIDKEY) {
				return;
			}
			Room* room = pServer->GetRooms()[roomid];
			if (room == nullptr) {
				return;
			}
			if (room->GetState() == eRoomState::RS_FREE) {
				return;
			}

			pServer->GetPacketSender()->SendBombExplosionPacket(roomid, bombid);
			room->DeleteBomb(bombid);
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};