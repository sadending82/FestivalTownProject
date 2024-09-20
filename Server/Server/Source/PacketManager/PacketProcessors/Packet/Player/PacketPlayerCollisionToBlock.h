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
				// �׷α� ���·� ����
				player->SetPlayerState(ePlayerState::PS_GROGGY);
				player->SetStamina(0);
				player->AddGroggyCount();
				pServer->GetPacketSender()->SendPlayerGroggyPacket(playerid, roomid);

				// ����ִ� ���� ����
				if (player->GetWeapon() != nullptr) {
					if (player->GetWeapon()->SetIsGrabbed(false) == true) {
						int weaponID = player->GetWeapon()->GetID();
						player->GetWeapon()->SetOwenrID(INVALIDKEY);
						player->SetWeapon(nullptr);
						pServer->GetPacketSender()->SendWeaponDropPacket(player->GetPosition(), roomid, weaponID);
					}
				}

				PushEventGroggyRecovery(pServer->GetTimer(), playerid, roomid, roomCode, player->GroggyRecoverTime());
			}
			else if (playerState == ePlayerState::PS_GROGGY) {
				// ����
				player->SetPlayerState(ePlayerState::PS_DEAD);
				pServer->GetPacketSender()->SendPlayerDeadPacket(playerid, roomid);

				// ����ִ� ���� ����
				if (player->GetWeapon() != nullptr) {
					if (player->GetWeapon()->SetIsGrabbed(false) == true) {
						int weaponID = player->GetWeapon()->GetID();
						player->GetWeapon()->SetOwenrID(INVALIDKEY);
						player->SetWeapon(nullptr);
						pServer->GetPacketSender()->SendWeaponDropPacket(player->GetPosition(), roomid, weaponID);
					}
				}

				// record update
				room->GetPlayerRecordList()[playerid].death_count++;

				int spawnTime = pServer->GetTableManager()->GetGameModeData()[room->GetGameMode()]->Player_Spawn_Time;
				PushEventPlayerRespawn(pServer->GetTimer(), playerid, roomid, roomCode, spawnTime);
			}
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};