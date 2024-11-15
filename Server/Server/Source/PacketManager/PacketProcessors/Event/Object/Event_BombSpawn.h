#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

using namespace PacketTable::ObjectTable;

class Event_BombSpawn : public PacketProcessor {

public:
	Event_BombSpawn(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}


	virtual void Process(unsigned char* buf) { 
		try {
			EV_BOMB_SPAWN* event = reinterpret_cast<EV_BOMB_SPAWN*>(buf);

			int roomID = event->roomID;
			Room* room = pServer->GetRooms().at(roomID);
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
			GameModeData& modeInfo = pServer->GetTableManager()->GetGameModeData()[gameMode];
			int nextEventTime = modeInfo.Bomb_Spawn_Time; // seconds

			pServer->GetGameManagers()[gameMode]->BombSpawn(room, roomID);

			PushEventBombSpawn(pServer->GetTimer(), event->roomID, event->roomCode, nextEventTime);
		}
		catch (const std::exception& e) {
			std::cerr << "[Event_BombSpawn ERROR] : " << e.what() << std::endl;
		}
	}

private:
};