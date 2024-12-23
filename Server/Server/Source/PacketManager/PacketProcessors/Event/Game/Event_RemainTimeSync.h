#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

class Event_RemainTimeSync : public PacketProcessor {

public:
	Event_RemainTimeSync(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(unsigned char* buf) { 
		try {
			EV_TIME_SYNC* event = reinterpret_cast<EV_TIME_SYNC*>(buf);

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

			PushEventRemainTimeSync(pServer->GetTimer(), roomID, event->roomCode);
			pPacketSender->SendRemainTimeSync(roomID);
		}
		catch (const std::exception& e) {
			std::cerr << "[Event_RemainTimeSync ERROR] : " << e.what() << std::endl;
		}
	}

private:
};