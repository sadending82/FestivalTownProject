#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

class Event_GameStart : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		try {
			EV_GAME_START* event = reinterpret_cast<EV_GAME_START*>(buf);

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

			pServer->GetGameManagers()[gameMode]->StartGame(roomid);
		}
		catch (const std::exception& e) {
			std::cerr << "[ERROR] : " << e.what() << std::endl;
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};