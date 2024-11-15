#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

class Event_PlayerRespawn : public PacketProcessor {

public:
	Event_PlayerRespawn(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(unsigned char* buf) { 
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

			int sessionID = room->GetPlayerList()[playerid].load();
			if (sessionID == INVALIDKEY) {
				return;
			}
			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[sessionID]);
			if (player == nullptr) {
				return;
			}

			if (room->GetTeams().at(player->GetTeam()).GetLife() <= 0) {
				return;
			}

			player->GetPlayerStateLock().lock();
			player->SetPlayerState(ePlayerState::PS_ALIVE);
			player->GetPlayerStateLock().unlock();
			player->SetHP(player->GetCharacterStat().hp);
			player->SetStamina(player->GetCharacterStat().stamina);
			
			player->GetWeaponLock().lock();
			player->SetWeapon(nullptr);
			player->GetWeaponLock().unlock();

			player->GetBombLock().lock();
			player->SetBomb(nullptr);
			player->GetBombLock().unlock();

			pPacketSender->SendPlayerRespawn(playerid, roomid);
		}
		catch (const std::exception& e) {
			std::cerr << "[Event_PlayerRespawn ERROR] : " << e.what() << std::endl;
		}
	}

private:
};