#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerCollisionToBlock : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {
		try {

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
				Room* room = pServer->GetRooms().at(roomid);
				long long roomCode = room->GetRoomCode();

				player->GetPlayerStateLock().lock();
				ePlayerState playerState = player->GetPlayerState();
				player->GetPlayerStateLock().unlock();

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
			std::cerr << "[ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};