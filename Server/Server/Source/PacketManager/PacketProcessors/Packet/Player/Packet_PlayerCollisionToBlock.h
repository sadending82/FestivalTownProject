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

				int targetID = read->id();
				Player* host = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				int roomID = host->GetRoomID();
				if (roomID == INVALIDKEY) {
					return;
				}
				Room* room = pServer->GetRooms().at(roomID);
				room->GetStateLock().lock_shared();
				if (room->GetState() != eRoomState::RS_INGAME) {
					room->GetStateLock().unlock_shared();
					return;
				}
				room->GetStateLock().unlock_shared();

				Player* target = dynamic_cast<Player*>(pServer->GetSessions()[room->GetPlayerList()[targetID]]);

				if (target == nullptr && target->GetInGameID() != targetID) {
					return;
				}

				long long roomCode = room->GetRoomCode();

				target->GetPlayerStateLock().lock_shared();
				ePlayerState playerState = target->GetPlayerState();
				target->GetPlayerStateLock().unlock_shared();

				sPlayerGameRecord& playerGameRecord = room->GetPlayerRecordList().at(targetID);

				if (playerState == ePlayerState::PS_ALIVE) {
					// 블록 충돌 데미지 계산
					int blockHitDamage = room->GetGameModeData().Block_Hit;
					target->ReduceHP(blockHitDamage);
					if (target->GetHP() <= 0) {
						int spawnTime = room->GetGameModeData().Player_Spawn_Time;
						if (target->ChangeToDeadState(pServer, spawnTime)) {
							playerGameRecord.gameRecord.DeathCount.fetch_add(1);
							PushEventPlayerRespawn(pServer->GetTimer(), targetID, roomID, room->GetRoomCode(), spawnTime);
						}
					}
					else {
						// 그로기 상태로 만듬
						if (target->ChangeToGroggyState(pServer)) {
							playerGameRecord.gameRecord.Groggy_Count.fetch_add(1);
							long long groggyTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
							target->SetLastGroggyTime(groggyTime);
							PushEventGroggyRecovery(pServer->GetTimer(), targetID, roomID, groggyTime, roomCode, room->GetGameModeData().Ch_Groggy);
							//COUT << targetID << " 블록맞고 그로기\n";
						}
					}
				}
				else if (playerState == ePlayerState::PS_GROGGY) {
					int spawnTime = room->GetGameModeData().Player_Spawn_Time;
					// 죽임
					if (target->ChangeToDeadState(pServer, spawnTime)) {
						playerGameRecord.gameRecord.DeathCount.fetch_add(1);
						PushEventPlayerRespawn(pServer->GetTimer(), targetID, roomID, roomCode, spawnTime);
						//COUT << targetID << " 블록맞고 사망\n";
					}
				}
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_PlayerCollisionToBlock ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};