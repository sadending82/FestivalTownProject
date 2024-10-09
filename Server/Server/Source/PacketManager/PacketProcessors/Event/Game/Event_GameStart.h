#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

class Event_GameStart : public PacketProcessor {

public:
	Event_GameStart(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(unsigned char* buf) {
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
			std::cerr << "[Event_GameStart ERROR] : " << e.what() << std::endl;
		}
	}

private:
};