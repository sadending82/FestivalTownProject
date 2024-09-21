#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class PacketPlayerSync : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<PlayerSync>(nullptr)) {

			const PlayerSync* read = flatbuffers::GetRoot<PlayerSync>(data);
			int playerid = read->id();

			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (player == nullptr && player->GetInGameID() != read->id()) {
				return;
			}

			player->SetPosition(read->pos()->x(), read->pos()->y(), read->pos()->z());
			player->SetDirection(read->direction()->x(), read->direction()->y(), read->direction()->z());
			player->SetStamina(read->stamina());

			std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYER_SYNC, data, size);

			pServer->SendAllPlayerInRoomExceptSender(send_buffer.data(), send_buffer.size(), key);

			if (read->stamina() <= 0) {
				player->GetPlayerStateLock().lock();
				ePlayerState playerState = player->GetPlayerState();
				player->GetPlayerStateLock().unlock();

				// �׷α� ���°� �ƴ϶�� �׷α� ���·� ����
				if (playerState == ePlayerState::PS_ALIVE) {

					player->GetPlayerStateLock().lock();
					player->SetPlayerState(ePlayerState::PS_GROGGY);
					player->GetPlayerStateLock().unlock();

					int roomid = player->GetRoomID();
					Room* room = pServer->GetRooms()[roomid];
					long long roomCode = room->GetRoomCode();

					player->AddGroggyCount();
					pServer->GetPacketSender()->SendPlayerGroggyPacket(playerid, roomid);

					// ����ִ� ���� ����
					Weapon* weapon = player->GetWeapon();
					if (weapon != nullptr) {
						if (weapon->SetIsGrabbed(false) == true) {
							int weaponID = weapon->GetID();
							weapon->SetOwenrID(INVALIDKEY);
							player->SetWeapon(nullptr);
							weapon->SetPosition(Vector3f(read->pos()->x(), read->pos()->y(), read->pos()->z()));
							pServer->GetPacketSender()->SendWeaponDropPacket(player->GetPosition(), roomid, weaponID);
						}
					}

					PushEventGroggyRecovery(pServer->GetTimer(), playerid, roomid, roomCode, player->GroggyRecoverTime());
				}
			}
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};