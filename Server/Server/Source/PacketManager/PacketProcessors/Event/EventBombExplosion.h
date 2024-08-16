#pragma once
#include "../PacketProcessor.h"
#include "../../../Event/Event.h"

using namespace PacketTable::ObjectTable;

class EventBombExplosion : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		EV_BOMB_EXPLOSION* event = reinterpret_cast<EV_BOMB_EXPLOSION*>(buf);

		int roomid = event->roomID;
		int bombid = event->bombID;
		if (pServer->GetRooms()[roomid]->GetState() == eRoomState::RS_FREE) {
			return;
		}

		//GameCode gameMode = pServer->GetRooms()[event->roomID]->GetGameMode();
		Bomb* bomb = dynamic_cast<Bomb*>(pServer->GetRooms()[roomid]->GetObjects()[bombid]);
		if (bomb == nullptr) return;
		pServer->SendBombExplosionPacket(roomid, bombid);
		bomb = nullptr;
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};