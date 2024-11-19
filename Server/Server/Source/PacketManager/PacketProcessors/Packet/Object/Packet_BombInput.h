#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::GameTable;

class Packet_BombInput : public PacketProcessor {

public:
	Packet_BombInput(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<BombInput>(nullptr)) {
				const BombInput* read = flatbuffers::GetRoot<BombInput>(data);

				int playerid = read->id();
				int bombid = read->bombid();
				int team = read->team();

				Player* host = dynamic_cast<Player*>(pServer->GetSessions()[key]);

				int roomID = host->GetRoomID();
				if (roomID == INVALIDKEY) {
					return;
				}
				Room* room = pServer->GetRooms().at(roomID);

				int player_sessionID = room->GetPlayerList()[playerid].load();

				if (player_sessionID == INVALIDKEY) {
					return;
				}

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[player_sessionID]);
				if (player == nullptr) {
					return;
				}
				GameMode gameMode = room->GetGameMode();
				GameManager* gameManager = pServer->GetGameManagers()[gameMode];
				sPlayerGameRecord& playerGameRecord = room->GetPlayerRecordList().at(playerid);

				room->DeleteBomb(bombid);

				// 자기 팀에 넣으면 바로 사망
				if (player->GetTeam() == team) {
					int spawnTime = room->GetGameModeData().Player_Spawn_Time;
					// 죽임
					player->ChangeToDeadState(pServer, spawnTime);

					// record update
					playerGameRecord.gameRecord.DeathCount.fetch_add(1);
					PushEventPlayerRespawn(pServer->GetTimer(), playerid, roomID, room->GetRoomCode(), spawnTime);
					return;
				}
				Team& teamInfo = room->GetTeams()[team];
				teamInfo.ReduceLife();
				int lifeCount = teamInfo.GetLife();
				pPacketSender->SendLifeReducePacket(team, lifeCount, roomID);

				// record update
				playerGameRecord.gameRecord.Bomb_Count.fetch_add(1);

				if (lifeCount <= 0) {
					// life가 0인데 게임이 안끝나면 바로 죽이고 관전상태로
					if (gameManager->CheckGameEnd(roomID) == false) {
						for (int member : teamInfo.GetMembers()) {
							int sessionID = room->GetPlayerList()[member].load();
							dynamic_cast<Player*>(pServer->GetSessions()[sessionID])->ChangeToDeadState(pServer, 0);
						}
					}
				}
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_BombInput ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};