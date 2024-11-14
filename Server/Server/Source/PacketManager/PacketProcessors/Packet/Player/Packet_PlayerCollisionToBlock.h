#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerCollisionToBlock : public PacketProcessor {

public:
	Packet_PlayerCollisionToBlock(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<PlayerCollisionToBlock>(nullptr)) {
				const PlayerCollisionToBlock* read = flatbuffers::GetRoot<PlayerCollisionToBlock>(data);

				int playerid = read->id();
				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				if (player == nullptr && player->GetInGameID() != read->id()) {
					return;
				}
				int roomid = player->GetRoomID();
				if (roomid == INVALIDKEY) {
					return;
				}
				Room* room = pServer->GetRooms().at(roomid);
				room->GetStateLock().lock_shared();
				if (room->GetState() != eRoomState::RS_INGAME) {
					room->GetStateLock().unlock_shared();
					return;
				}
				room->GetStateLock().unlock_shared();
				long long roomCode = room->GetRoomCode();

				player->GetPlayerStateLock().lock_shared();
				ePlayerState playerState = player->GetPlayerState();
				player->GetPlayerStateLock().unlock_shared();

				sPlayerGameRecord& playerGameRecord = room->GetPlayerRecordList().at(playerid);

				if (playerState == ePlayerState::PS_ALIVE) {
					// ±×·Î±â »óÅÂ·Î ¸¸µë
					player->ChangeToGroggyState(pServer);
					playerGameRecord.gameRecord.Groggy_Count.fetch_add(1);
					PushEventGroggyRecovery(pServer->GetTimer(), playerid, roomid, roomCode, room->GetGameModeData().Ch_Groggy);
				}
				else if (playerState == ePlayerState::PS_GROGGY) {
					int spawnTime = room->GetGameModeData().Player_Spawn_Time;
					// Á×ÀÓ
					player->ChangeToDeadState(pServer, spawnTime);

					// record update
					playerGameRecord.gameRecord.DeathCount.fetch_add(1);
					PushEventPlayerRespawn(pServer->GetTimer(), playerid, roomid, roomCode, spawnTime);
				}
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_PlayerCollisionToBlock ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};