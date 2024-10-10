#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerCollisionToBlock : public PacketProcessor {

public:
	Packet_PlayerCollisionToBlock(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {

			mBuilder.Clear();

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

				if (playerState == ePlayerState::PS_ALIVE) {
					// ±×·Î±â »óÅÂ·Î ¸¸µë
					player->ChangeToGroggyState(pServer);
					PushEventGroggyRecovery(pServer->GetTimer(), playerid, roomid, roomCode, player->GroggyRecoverTime());
				}
				else if (playerState == ePlayerState::PS_GROGGY) {
					int spawnTime = room->GetGameModeData().Player_Spawn_Time;
					// Á×ÀÓ
					player->ChangeToDeadState(pServer, spawnTime);

					// record update
					room->GetPlayerRecordList().at(playerid).death_count++;
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