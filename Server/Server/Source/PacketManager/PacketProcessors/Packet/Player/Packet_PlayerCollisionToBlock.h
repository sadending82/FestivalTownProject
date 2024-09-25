#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerCollisionToBlock : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<PlayerStop>(nullptr)) {


			const PlayerCollisionToBlock* read = flatbuffers::GetRoot<PlayerCollisionToBlock>(data);

			int playerid = read->id();
			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (player == nullptr && player->GetInGameID() != read->id()) {
				return;
			}
			int roomid = player->GetRoomID();
			Room* room = pServer->GetRooms()[roomid];
			long long roomCode = room->GetRoomCode();

			player->GetPlayerStateLock().lock();
			ePlayerState playerState = player->GetPlayerState();
			player->GetPlayerStateLock().unlock();

			if (playerState == ePlayerState::PS_ALIVE) {
				// ±×·Î±â »óÅÂ·Î ¸¸µë
				player->ChangeToGroggyState(pServer, roomid);
				PushEventGroggyRecovery(pServer->GetTimer(), playerid, roomid, roomCode, player->GroggyRecoverTime());
			}
			else if (playerState == ePlayerState::PS_GROGGY) {
				// Á×ÀÓ
				player->ChangeToDeadState(pServer, roomid);

				// record update
				room->GetPlayerRecordList()[playerid].death_count++;

				int spawnTime = pServer->GetTableManager()->GetGameModeData()[room->GetGameMode()].Player_Spawn_Time;
				PushEventPlayerRespawn(pServer->GetTimer(), playerid, roomid, roomCode, spawnTime);
			}
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};