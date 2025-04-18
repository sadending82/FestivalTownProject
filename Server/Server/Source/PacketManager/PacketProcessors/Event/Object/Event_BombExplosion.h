#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

using namespace PacketTable::ObjectTable;

class Event_BombExplosion : public PacketProcessor {

public:
	Event_BombExplosion(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(unsigned char* buf) {
		try {
			EV_BOMB_EXPLOSION* event = reinterpret_cast<EV_BOMB_EXPLOSION*>(buf);

			int roomID = event->roomID;
			int bombid = event->bombID;
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

			Bomb* bomb = room->GetBomb(bombid);

			if (bomb == nullptr) {
				return;
			}

			if (bomb->GetUniqueCode() != event->bombUniqueCode) {
				return;
			}

			pPacketSender->SendBombExplosionPacket(roomID, bombid);
			room->DeleteBomb(bombid);
		}
		catch (const std::exception& e) {
			std::cerr << "[Event_BombExplosion ERROR] : " << e.what() << std::endl;
		}
	}

private:
};