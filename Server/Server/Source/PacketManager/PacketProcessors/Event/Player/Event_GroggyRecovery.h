#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

class Event_GroggyRecovery: public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		EV_GROGGY_RECOVERY* event = reinterpret_cast<EV_GROGGY_RECOVERY*>(buf);

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

		CharacterStat& characterStat = pServer->GetTableManager()->GetCharacterStats()[(int)player->GetChacracterType()];

		int staminaRecoveryValue = characterStat.stamina;

		player->SetStamina(characterStat.stamina);
		player->GetPlayerStateLock().lock();
		player->SetPlayerState(ePlayerState::PS_ALIVE);
		player->GetPlayerStateLock().unlock();

		pServer->GetPacketSender()->SendPlayerGroggyRecoveryPacket(playerid, roomid, staminaRecoveryValue);
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};