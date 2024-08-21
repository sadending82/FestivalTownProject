#pragma once
#include "../PacketProcessor.h"
#include "../../../Event/Event.h"

class EventPlayerRespawn : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		EV_PLAYER_RESPAWN* event = reinterpret_cast<EV_PLAYER_RESPAWN*>(buf);

		int roomid = event->roomID;
		int playerid = event->playerID;
		Room* room = pServer->GetRooms()[roomid];
		long long roomCode = room->GetRoomCode();
		if (roomCode != event->roomCode) {
			return;
		}
		if (room->GetState() == eRoomState::RS_FREE) {
			return;
		}

		pServer->SendRemainTimeSync(roomid);
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};