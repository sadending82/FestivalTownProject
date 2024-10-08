#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::GameTable;

class Packet_BombInput : public PacketProcessor {

public:
	Packet_BombInput(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			mBuilder.Clear();

			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<BombInput>(nullptr)) {
				const BombInput* read = flatbuffers::GetRoot<BombInput>(data);

				int playerid = read->id();
				int bombid = read->bombid();
				int team = read->team();

				int roomid = dynamic_cast<Player*>(pServer->GetSessions()[key])->GetRoomID();
				Room* room = pServer->GetRooms().at(roomid);
				GameMode gameMode = room->GetGameMode();
				GameManager* gameManager = pServer->GetGameManagers()[gameMode];

				room->GetPlayerListLock().lock_shared();
				Player* player = dynamic_cast<Player*>(room->GetPlayerList().at(playerid));
				if (player == nullptr) {
					room->GetPlayerListLock().unlock_shared();
					return;
				}
				room->GetPlayerListLock().unlock_shared();

				room->DeleteBomb(bombid);

				// ÀÚ±â ÆÀ¿¡ ³ÖÀ¸¸é ¹Ù·Î »ç¸Á
				if (player->GetTeam() == team) {
					int spawnTime = room->GetGameModeData().Player_Spawn_Time;
					// Á×ÀÓ
					player->ChangeToDeadState(pServer, spawnTime);

					// record update
					room->GetPlayerRecordList().at(playerid).death_count++;
					PushEventPlayerRespawn(pServer->GetTimer(), playerid, roomid, room->GetRoomCode(), spawnTime);
					return;
				}

				room->GetTeams()[team].ReduceLife();
				int lifeCount = room->GetTeams()[team].GetLife();
				pPacketSender->SendLifeReducePacket(team, lifeCount, roomid);

				// record update
				room->GetPlayerRecordList().at(playerid).bomb_insert_count++;

				if (lifeCount <= 0) {
					gameManager->CheckGameEnd(roomid);
				}
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};