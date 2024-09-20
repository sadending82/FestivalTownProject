#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::GameTable;

class PacketBombInput : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<BombInput>(nullptr)) {
			const BombInput* read = flatbuffers::GetRoot<BombInput>(data);

			int playerid = read->id();
			int bombid = read->bombid();
			int team = read->team();

			int roomid = dynamic_cast<Player*>(pServer->GetSessions()[key])->GetRoomID();
			Room* room = pServer->GetRooms()[roomid];

			room->GetPlayerListLock().lock_shared();
			Player* player = dynamic_cast<Player*>(room->GetPlayerList()[playerid]);
			if (player == nullptr) {
				room->GetPlayerListLock().unlock_shared();
				return;
			}
			room->GetPlayerListLock().unlock_shared();

			room->DeleteBomb(bombid);

			// 자기 팀에 넣으면 바로 사망
			if (player->GetTeam() == team) {
				pServer->GetPacketSender()->SendPlayerDeadPacket(playerid, roomid);
				// record update
				room->GetPlayerRecordList()[playerid].death_count++;
				int spawnTime = pServer->GetTableManager()->GetGameModeData()[room->GetGameMode()].Player_Spawn_Time;
				PushEventPlayerRespawn(pServer->GetTimer(), playerid, roomid, room->GetRoomCode(), spawnTime);
				return;
			}

			pServer->GetRooms()[roomid]->GetTeams()[team].ReduceLife();
			int lifeCount = pServer->GetRooms()[roomid]->GetTeams()[team].GetLife();
			pServer->GetPacketSender()->SendLifeReducePacket(team, lifeCount, roomid);

			// record update
			room->GetPlayerRecordList()[playerid].bomb_insert_count++;

			if (lifeCount <= 0) {
				pServer->CheckGameEnd(roomid);
			}
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};