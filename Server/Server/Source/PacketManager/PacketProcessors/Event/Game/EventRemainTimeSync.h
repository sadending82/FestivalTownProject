#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

class EventRemainTimeSync : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		EV_TIME_SYNC* event = reinterpret_cast<EV_TIME_SYNC*>(buf);

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

		GameMode gameMode = room->GetGameMode();
		
		int spawnCnt = pServer->GetTableManager()-> GetGameModeData()[gameMode]->Bomb_Spawn_Count;

		PushEventRemainTimeSync(pServer->GetTimer(), roomid, event->roomCode);
		pServer->GetPacketSender()->SendRemainTimeSync(roomid);
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};