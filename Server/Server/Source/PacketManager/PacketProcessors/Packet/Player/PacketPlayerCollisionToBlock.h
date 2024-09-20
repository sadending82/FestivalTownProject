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

			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (player == nullptr && player->GetInGameID() != read->id()) {
				return;
			}
			int roomid = player->GetRoomID();
			Room* room = pServer->GetRooms()[roomid];

			player->GetPlayerStateLock().lock();
			ePlayerState playerState = player->GetPlayerState();
			if (playerState == ePlayerState::PS_ALIVE) {
				// 그로기 상태로 만듬
				player->SetPlayerState(ePlayerState::PS_GROGGY);

				// 들고있는 무기 해제
				if (player->GetWeapon() != nullptr) {
					if (player->GetWeapon()->SetIsGrabbed(false) == true) {
						int weaponID = player->GetWeapon()->GetID();
						player->GetWeapon()->SetOwenrID(INVALIDKEY);
						player->SetWeapon(nullptr);
						pServer->GetPacketSender()->SendWeaponDropPacket(player->GetPosition(), roomid, weaponID);
					}
				}

			}
			else if (playerState == ePlayerState::PS_GROGGY) {
				// 죽임
				player->SetPlayerState(ePlayerState::PS_DEAD);
				pServer->GetPacketSender()->SendPlayerDeadPacket(read->id(), roomid);

				// 들고있는 무기 해제
				if (player->GetWeapon() != nullptr) {
					if (player->GetWeapon()->SetIsGrabbed(false) == true) {
						int weaponID = player->GetWeapon()->GetID();
						player->GetWeapon()->SetOwenrID(INVALIDKEY);
						player->SetWeapon(nullptr);
						pServer->GetPacketSender()->SendWeaponDropPacket(player->GetPosition(), roomid, weaponID);
					}
				}

				// record update
				room->GetPlayerRecordList()[read->id()].death_count++;

				int spawnTime = pServer->GetTableManager()->GetGameModeData()[room->GetGameMode()]->Player_Spawn_Time;
				PushEventPlayerRespawn(pServer->GetTimer(), read->id(), roomid, room->GetRoomCode(), spawnTime);
			}
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};