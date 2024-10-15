#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

class Event_GroggyRecovery: public PacketProcessor {

public:
	Event_GroggyRecovery(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(unsigned char* buf) { 
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

			int sessionID = room->GetPlayerList()[playerid].load();
			if (sessionID == INVALIDKEY) {
				return;
			}

			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[sessionID]);
			if (player == nullptr) {
				return;
			}

			// ���������� Ǯ����
			if (player->SetIsGrabbed(false) == true) {
				int grabberID = player->GetAttachedPlayerID();

				int grabber_sessionID = room->GetPlayerList()[grabberID].load();
				if (grabber_sessionID == INVALIDKEY) {
					return;
				}

				Player* grabber = player = dynamic_cast<Player*>(pServer->GetSessions()[grabber_sessionID]);

				player->SetAttachedPlayerID(INVALIDKEY);
				grabber->SetAttachedPlayerID(INVALIDKEY);

				pPacketSender->SendPlayerThrowOtherPlayerPacket(roomid, grabberID, grabber->GetPosition(), grabber->GetDirection()
					, playerid, player->GetPosition(), player->GetDirection());
			}


			// ���׹̳� ȸ��
			CharacterStat& characterStat = player->GetCharacterStat();

			int staminaRecoveryValue = characterStat.stamina;

			player->SetStamina(characterStat.stamina);
			player->GetPlayerStateLock().lock();
			player->SetPlayerState(ePlayerState::PS_ALIVE);
			player->GetPlayerStateLock().unlock();

			pPacketSender->SendPlayerGroggyRecoveryPacket(playerid, roomid, staminaRecoveryValue);
		}
		catch (const std::exception& e) {
			std::cerr << "[Event_GroggyRecovery ERROR] : " << e.what() << std::endl;
		}
	}

private:
};