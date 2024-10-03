#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

using namespace PacketTable::ObjectTable;

class Event_BombSpawn : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) { 
		try {
			EV_BOMB_SPAWN* event = reinterpret_cast<EV_BOMB_SPAWN*>(buf);

			int roomid = event->roomID;
			Room* room = pServer->GetRooms().at(roomid);
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
			GameModeInfo& modeInfo = pServer->GetTableManager()->GetGameModeData()[gameMode];
			int nextEventTime = modeInfo.Bomb_Spawn_Time; // seconds

			pServer->GetGameManagers()[gameMode]->BombSpawn(room, roomid);

			PushEventBombSpawn(pServer->GetTimer(), event->roomID, event->roomCode, nextEventTime);
		}
		catch (const std::exception& e) {
			std::cerr << "[ERROR] : " << e.what() << std::endl;
		}
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};