#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

class EventRecoveryStamina : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		EV_RECOVERY_STAMINA* event = reinterpret_cast<EV_RECOVERY_STAMINA*>(buf);

		int roomid = event->roomID;
		int playerid = event->playerID;
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

		room->GetPlayerListLock().lock_shared();
		Player* player = room->GetPlayerList()[playerid];
		room->GetPlayerListLock().unlock_shared();
		if (player == nullptr) {
			return;
		}

		player->SetStamina(pServer->GetTableManager()->GetCharacterStats()[(int)eCharacterType::CT_TEST]->stamina);
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};