#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class PacketPlayerDamageReceive : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<PlayerDamageReceive>(nullptr)) {

			const PlayerDamageReceive* read = flatbuffers::GetRoot<PlayerDamageReceive>(data);

			int roomid = dynamic_cast<Player*>(pServer->GetSessions()[key])->GetRoomID();
			Room* room = pServer->GetRooms()[roomid];
			room->GetPlayerListLock().lock_shared();
			Player* target = room->GetPlayerList()[read->target_id()];
			Player* attacker = room->GetPlayerList()[read->attacker_id()];
			room->GetPlayerListLock().unlock_shared();
			if (target == nullptr || attacker == nullptr) {
				return;
			}

			// 살아있는지 확인
			target->GetPlayerStateLock().lock();
			if (target->GetPlayerState() != ePlayerState::PS_ALIVE) {
				target->GetPlayerStateLock().unlock();
				return;
			}

			// Game Data를 가지고 데미지 계산 필요
			int damageAmount;
			if (read->attack_type() == AT_FALLDOWN) {
				// 낙사 시 즉사
				damageAmount = 999;
			}
			else {
				damageAmount = pServer->GetTableManager()->getCharacterStats()[(int)CharacterType::TEST]->strength; // 임시
			}
			target->ReduceHP(damageAmount);

			if (target->GetHP() <= 0) {
				target->SetPlayerState(ePlayerState::PS_DEAD);
				int spawnTime = pServer->GetTableManager()->getFITH_Data()[room->GetGameMode()]->Player_Spawn_Time;
				pServer->GetPacketSender()->SendPlayerDeadPacket(read->target_id(), roomid);
				if (target->GetWeapon() != nullptr) {
					if (target->GetWeapon()->SetIsGrabbed(false) == true) {
						int weaponID = target->GetWeapon()->GetID();
						target->GetWeapon()->SetOwenrID(INVALIDKEY);
						target->SetWeapon(nullptr);
						pServer->GetPacketSender()->SendWeaponDropPacket(target->GetPosition(), roomid, weaponID);
					}
				}
				PushEventPlayerRespawn(pServer->GetTimer(), read->target_id(), roomid, room->GetRoomCode(), spawnTime);
			}
			else {
				Vector3f knockback_direction(read->knockback_direction()->x(), read->knockback_direction()->y(), read->knockback_direction()->z());
				pServer->GetPacketSender()->SendPlayerCalculatedDamage(read->target_id(), roomid, read->attack_type(), target->GetHP(), damageAmount, knockback_direction);
			}
			target->GetPlayerStateLock().unlock();
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};