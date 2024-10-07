#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

class Event_PlayerRespawn : public PacketProcessor {

public:
	virtual void Process(Server* pServer, unsigned char* buf) { 
		try {

			EV_PLAYER_RESPAWN* event = reinterpret_cast<EV_PLAYER_RESPAWN*>(buf);

			int roomid = event->roomID;
			int playerid = event->playerID;
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

			room->GetPlayerListLock().lock_shared();
			Player* player = room->GetPlayerList()[playerid];
			room->GetPlayerListLock().unlock_shared();
			if (player == nullptr) {
				return;
			}

			player->GetPlayerStateLock().lock();
			player->SetPlayerState(ePlayerState::PS_ALIVE);
			player->GetPlayerStateLock().unlock();
			player->SetHP(player->GetCharacterStat().hp);
			pServer->GetPacketSender()->SendPlayerRespawn(playerid, roomid);
		}
		catch (const std::exception& e) {
			std::cerr << "[ERROR] : " << e.what() << std::endl;
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};