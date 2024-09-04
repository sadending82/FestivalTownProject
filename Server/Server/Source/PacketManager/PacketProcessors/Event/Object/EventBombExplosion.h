#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

using namespace PacketTable::ObjectTable;

class EventBombExplosion : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		EV_BOMB_EXPLOSION* event = reinterpret_cast<EV_BOMB_EXPLOSION*>(buf);

		int roomid = event->roomID;
		int bombid = event->bombID;
		Room* room = pServer->GetRooms()[roomid];
		if (room == nullptr) {
			return;
		}
		long long roomCode = room->GetRoomCode();
		if (roomCode != event->roomCode) {
			return;
		}
		if (room->GetState() == eRoomState::RS_FREE) {
			return;
		}

		pServer->SendBombExplosionPacket(roomid, bombid);
		room->DeleteBomb(bombid);
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};