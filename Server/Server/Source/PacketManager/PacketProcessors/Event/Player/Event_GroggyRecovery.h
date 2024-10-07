#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

class Event_GroggyRecovery: public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) { 
		try {
			EV_GROGGY_RECOVERY* event = reinterpret_cast<EV_GROGGY_RECOVERY*>(buf);

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
			if (player == nullptr) {
				room->GetPlayerListLock().unlock_shared();
				return;
			}

			// 잡혀있으면 풀어줌
			if (player->SetIsGrabbed(false) == true) {
				int grabberID = player->GetAttachedPlayerID();
				Player* grabber = room->GetPlayerList()[player->GetAttachedPlayerID()];
				if (grabber == nullptr) {
					room->GetPlayerListLock().unlock_shared();
					return;
				}

				player->SetAttachedPlayerID(INVALIDKEY);
				grabber->SetAttachedPlayerID(INVALIDKEY);

				pServer->GetPacketSender()->SendPlayerThrowOtherPlayerPacket(roomid, grabberID, grabber->GetPosition(), grabber->GetDirection()
					, playerid, player->GetPosition(), player->GetDirection());
			}


			// 스테미나 회복
			CharacterStat& characterStat = player->GetCharacterStat();

			int staminaRecoveryValue = characterStat.stamina;

			player->SetStamina(characterStat.stamina);
			player->GetPlayerStateLock().lock();
			player->SetPlayerState(ePlayerState::PS_ALIVE);
			player->GetPlayerStateLock().unlock();

			room->GetPlayerListLock().unlock_shared();

			pServer->GetPacketSender()->SendPlayerGroggyRecoveryPacket(playerid, roomid, staminaRecoveryValue);
		}
		catch (const std::exception& e) {
			std::cerr << "[ERROR] : " << e.what() << std::endl;
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};