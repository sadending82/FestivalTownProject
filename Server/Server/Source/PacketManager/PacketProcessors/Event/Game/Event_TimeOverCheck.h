#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

class Event_TimeOverCheck : public PacketProcessor {

public:
	Event_TimeOverCheck(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(unsigned char* buf) { 
		try {
			EV_TIMEOVER_CHECK* event = reinterpret_cast<EV_TIMEOVER_CHECK*>(buf);

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
			GameManager* gameManager = pServer->GetGameManagers()[gameMode];

			TIMEPOINT startTime = room->GetStartTime();
			std::chrono::seconds playTime(room->GetGameModeData().Play_Time);
			TIMEPOINT endTime = startTime + playTime;
			TIMEPOINT currTime = std::chrono::system_clock::now();
			if (endTime <= currTime) {
				gameManager->TimeoverGameEnd(roomID);
				return;
			}

			PushEventTimeOverCheck(pServer->GetTimer(), roomID, event->roomCode);
		}
		catch (const std::exception& e) {
			std::cerr << "[Event_TimeOverCheck ERROR] : " << e.what() << std::endl;
		}
	}

private:
};