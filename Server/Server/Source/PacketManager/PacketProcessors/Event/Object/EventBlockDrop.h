#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

using namespace PacketTable::ObjectTable;

class EventBlockDrop : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		EV_OBJECT_DROP* event = reinterpret_cast<EV_OBJECT_DROP*>(buf);

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
		int nextEventTime = pServer->GetTableManager()-> GetGameModeData()[gameMode]->Block_Spawn_Time; // seconds
		int spawnCnt = pServer->GetTableManager()-> GetGameModeData()[gameMode]->Block_Spawn_Count;
		PushEventBlockDrop(pServer->GetTimer(), event->roomID, event->roomCode, nextEventTime);

		pServer->GetPacketSender()->SendBlockDropPacket(event->roomID, spawnCnt);
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};