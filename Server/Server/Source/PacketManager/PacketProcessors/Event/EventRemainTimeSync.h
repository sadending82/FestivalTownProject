#pragma once
#include "../PacketProcessor.h"
#include "../../../Event/Event.h"

class EventRemainTimeSync : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		EV_TIME_SYNC* event = reinterpret_cast<EV_TIME_SYNC*>(buf);

		int roomid = event->roomID;
		if (pServer->GetRooms()[roomid]->GetState() == eRoomState::RS_FREE) {
			return;
		}

		GameCode gameMode = pServer->GetRooms()[event->roomID]->GetGameMode();
		
		int spawnCnt = pServer->GetTableManager()->getFITH_Data()[gameMode].Bomb_Spawn_Count;

		PushEventRemainTimeSync(pServer->GetTimer(), roomid);
		pServer->SendRemainTimeSync(roomid);
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};