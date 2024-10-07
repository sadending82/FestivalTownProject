#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

class Event_TimeOverCheck : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) { 
		try {
			EV_TIMEOVER_CHECK* event = reinterpret_cast<EV_TIMEOVER_CHECK*>(buf);

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
			GameManager* gameManager = pServer->GetGameManagers()[gameMode];

			TIMEPOINT startTime = room->GetStartTime();
			std::chrono::seconds playTime(room->GetGameModeData().Play_Time);
			TIMEPOINT endTime = startTime + playTime;
			TIMEPOINT currTime = std::chrono::system_clock::now();
			if (endTime <= currTime) {
				gameManager->TimeoverGameEnd(roomid);
				return;
			}

			PushEventTimeOverCheck(pServer->GetTimer(), roomid, event->roomCode);
		}
		catch (const std::exception& e) {
			std::cerr << "[ERROR] : " << e.what() << std::endl;
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};