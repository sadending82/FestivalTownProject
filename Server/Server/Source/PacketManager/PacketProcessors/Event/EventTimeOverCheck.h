#pragma once
#include "../PacketProcessor.h"
#include "../../../Event/Event.h"

class EventTimeOverCheck : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		EV_TIMEOVER_CHECK* event = reinterpret_cast<EV_TIMEOVER_CHECK*>(buf);

		int roomid = event->roomID;
		Room* room = pServer->GetRooms()[roomid];
		long long roomCode = room->GetRoomCode();
		if (roomCode != event->roomCode) {
			return;
		}
		if (room->GetState() == eRoomState::RS_FREE) {
			return;
		}

		GameCode gameMode = room->GetGameMode();

		TIMEPOINT startTime = room->GetStartTime();
		std::chrono::seconds playTime(pServer->GetTableManager()->getFITH_Data()[gameMode].Play_Time);
		TIMEPOINT endTime = startTime + playTime;
		TIMEPOINT currTime = std::chrono::system_clock::now();
		if (endTime <= currTime) {
			// ���� ���� ó��
			return;
		}

		PushEventTimeOverCheck(pServer->GetTimer(), roomid, event->roomCode);
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};