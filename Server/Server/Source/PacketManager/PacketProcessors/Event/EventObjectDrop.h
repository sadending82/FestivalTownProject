#pragma once
#include "../PacketProcessor.h"
#include "../../../Event/Event.h"

using namespace PacketTable::ObjectTable;

class EventObjectDrop : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		// 방이 활성화 되어있는지 확인해야함
		EV_OBJECT_DROP* event = reinterpret_cast<EV_OBJECT_DROP*>(buf);

		int roomid = event->roomID;
		if (pServer->GetRooms()[roomid]->GetState() == eRoomState::ST_FREE) {
			return;
		}

		GameCode gameMode = pServer->GetRooms()[event->roomID]->GetGameMode();
		int nextEventTime = pServer->GetTableManager()->getFITH_Data()[gameMode].Block_Spawn_Time; // seconds
		int spawnCnt = pServer->GetTableManager()->getFITH_Data()[gameMode].Block_Spawn_Count;
		PushEventObjectDrop(pServer->GetTimer(), event->roomID, nextEventTime);

		pServer->SendObjectDropPacket(event->roomID, spawnCnt);
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};