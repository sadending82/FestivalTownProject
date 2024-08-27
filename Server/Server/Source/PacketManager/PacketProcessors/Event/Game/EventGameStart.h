#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

class EventGameStart : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		EV_GAME_START* event = reinterpret_cast<EV_GAME_START*>(buf);

		int roomid = event->roomID;
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

		pServer->StartGame(roomid);
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};