#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

class Event_TimeOverCheck : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		EV_TIMEOVER_CHECK* event = reinterpret_cast<EV_TIMEOVER_CHECK*>(buf);

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

		TIMEPOINT startTime = room->GetStartTime();
		std::chrono::seconds playTime(pServer->GetTableManager()-> GetGameModeData()[gameMode].Play_Time);
		TIMEPOINT endTime = startTime + playTime;
		TIMEPOINT currTime = std::chrono::system_clock::now();
		if (endTime <= currTime) {
			pServer->TimeoverGameEnd(roomid);
			return;
		}

		PushEventTimeOverCheck(pServer->GetTimer(), roomid, event->roomCode);
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};