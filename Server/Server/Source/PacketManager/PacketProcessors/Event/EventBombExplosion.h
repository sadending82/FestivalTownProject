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
		Room* room = pServer->GetRooms()[roomid];
		long long roomCode = room->GetRoomCode();
		if (roomCode != event->roomCode) {
			return;
		}
		if (room->GetState() == eRoomState::RS_FREE) {
			return;
		}

		room->GetObjectListLock().lock();
		Bomb* bomb = dynamic_cast<Bomb*>(room->GetObjects()[bombid]);
		if (bomb == nullptr) {
			room->GetObjectListLock().unlock();
			return;
		}
		pServer->SendBombExplosionPacket(roomid, bombid);
		pServer->GetRooms()[roomid]->GetObjects()[bombid] = nullptr;
		room->GetObjectListLock().unlock();
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};