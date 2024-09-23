#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class PacketPlayerCollisionToBlock : public PacketProcessor {

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

			if (playerState == ePlayerState::PS_ALIVE) {
				// 그로기 상태로 만듬
				player->SetPlayerState(ePlayerState::PS_GROGGY);
				player->SetStamina(0);
				player->AddGroggyCount();
				pServer->GetPacketSender()->SendPlayerGroggyPacket(playerid, roomid);

				// 들고있는 무기 해제
				Weapon* weapon = player->GetWeapon();
				if (weapon != nullptr) {
					if (weapon->SetIsGrabbed(false) == true) {
						int weaponID = weapon->GetID();
						weapon->SetOwenrID(INVALIDKEY);
						player->SetWeapon(nullptr);
						weapon->SetPosition(player->GetPosition());
						pServer->GetPacketSender()->SendWeaponDropPacket(player->GetPosition(), roomid, weaponID);
					}
				}

				PushEventGroggyRecovery(pServer->GetTimer(), playerid, roomid, roomCode, player->GroggyRecoverTime());
			}
			else if (playerState == ePlayerState::PS_GROGGY) {
				// 죽임
				player->SetPlayerState(ePlayerState::PS_DEAD);
				pServer->GetPacketSender()->SendPlayerDeadPacket(playerid, roomid);

				// 들고있는 무기 해제
				Weapon* weapon = player->GetWeapon();
				if (weapon != nullptr) {
					if (weapon->SetIsGrabbed(false) == true) {
						int weaponID = weapon->GetID();
						weapon->SetOwenrID(INVALIDKEY);
						weapon->SetPosition(player->GetPosition());
						player->SetWeapon(nullptr);
						pServer->GetPacketSender()->SendWeaponDropPacket(player->GetPosition(), roomid, weaponID);
					}
				}

				// record update
				room->GetPlayerRecordList()[playerid].death_count++;

				int spawnTime = pServer->GetTableManager()->GetGameModeData()[room->GetGameMode()].Player_Spawn_Time;
				PushEventPlayerRespawn(pServer->GetTimer(), playerid, roomid, roomCode, spawnTime);
			}

			player->GetPlayerStateLock().unlock();
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};