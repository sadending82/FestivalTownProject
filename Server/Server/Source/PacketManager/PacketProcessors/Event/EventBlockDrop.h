#pragma once
#include "../PacketProcessor.h"
#include "../../../Event/Event.h"

using namespace PacketTable::ObjectTable;

class EventBlockDrop : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		EV_OBJECT_DROP* event = reinterpret_cast<EV_OBJECT_DROP*>(buf);

		int roomid = event->roomID;
		if (pServer->GetRooms()[roomid]->GetState() == eRoomState::RS_FREE) {
			return;
		}

		GameCode gameMode = pServer->GetRooms()[event->roomID]->GetGameMode();
		int nextEventTime = pServer->GetTableManager()->getFITH_Data()[gameMode].Block_Spawn_Time; // seconds
		int spawnCnt = pServer->GetTableManager()->getFITH_Data()[gameMode].Block_Spawn_Count;
		PushEventBlockDrop(pServer->GetTimer(), event->roomID, nextEventTime);

		pServer->SendBlockDropPacket(event->roomID, spawnCnt);
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};