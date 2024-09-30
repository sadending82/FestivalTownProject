#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerDamageReceive : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<PlayerDamageReceive>(nullptr)) {

			const PlayerDamageReceive* read = flatbuffers::GetRoot<PlayerDamageReceive>(data);

			int attacker_id = read->attacker_id();
			int target_id = read->target_id();
			int roomid = dynamic_cast<Player*>(pServer->GetSessions()[key])->GetRoomID();
			Room* room = pServer->GetRooms()[roomid];

			room->GetPlayerListLock().lock_shared();
			Player* target = room->GetPlayerList()[read->target_id()];
			Player* attacker = room->GetPlayerList()[read->attacker_id()];

			if (target == nullptr || attacker == nullptr) {
				return;
			}

			target->GetPlayerStateLock().lock();
			// 살아있는지 확인
			if (target->GetPlayerState() != ePlayerState::PS_ALIVE) {
				target->GetPlayerStateLock().unlock();
				room->GetPlayerListLock().unlock_shared();
				return;
			}
			target->GetPlayerStateLock().unlock();

			TableManager* tableManager = pServer->GetTableManager();
			CharacterStat& attackerStat = tableManager->GetCharacterStats()[(int)attacker->GetChacracterType()];

			int damageAmount = 0;
			int TargetStaminaLoss = 0;

			switch (read->attack_type()) 
			{
			case eDamageType::AT_FALLDOWN: {
				damageAmount = 9999999;
				// 사망 처리
				target->ReduceHP(damageAmount);
				if (target->ChangeToDeadState(pServer, roomid)) {
					// record update
					room->GetPlayerRecordList()[target_id].death_count++;
					int spawnTime = tableManager->GetGameModeData()[room->GetGameMode()].Player_Spawn_Time;
					PushEventPlayerRespawn(pServer->GetTimer(), target_id, roomid, room->GetRoomCode(), spawnTime);
				}
			}break;

			case eDamageType::AT_ATTACK: {
				if (target_id == attacker_id) {
					room->GetPlayerListLock().unlock_shared();
					return;
				}

				// 타겟의 기력이 없으면 그로기 상태로
				if (target->GetStamina() == 0) {
					if (target->ChangeToGroggyState(pServer, roomid)) {
						PushEventGroggyRecovery(pServer->GetTimer(), target_id, roomid, room->GetRoomCode(), target->GroggyRecoverTime());
					}
				}

				// 데미지 계산
				damageAmount += attackerStat.strength * tableManager->GetAttackStats()[eDamageType::AT_ATTACK].Value;
				if (attacker->GetWeapon() != nullptr) {
					damageAmount += tableManager->GetWeaponStats()[(int)attacker->GetWeapon()->GetType()].Weapon_Power;
				}
				TargetStaminaLoss += tableManager->GetAttackStats()[eDamageType::AT_ATTACK].Steal_Stamina;

				// 데미지 적용
				target->ReduceHP(damageAmount);
				target->ReduceStamina(TargetStaminaLoss);

				if (target->GetHP() <= 0) {
					// 사망 처리
					target->ChangeToDeadState(pServer, roomid);

					// record update
					room->GetPlayerRecordList()[target_id].death_count++;
					room->GetPlayerRecordList()[attacker_id].kill_count++;
					int spawnTime = tableManager->GetGameModeData()[room->GetGameMode()].Player_Spawn_Time;
					PushEventPlayerRespawn(pServer->GetTimer(), target_id, roomid, room->GetRoomCode(), spawnTime);
				}
				else {
					Vector3f knockback_direction(read->knockback_direction()->x(), read->knockback_direction()->y(), read->knockback_direction()->z());
					pServer->GetPacketSender()->SendPlayerCalculatedDamage(target_id, roomid, read->attack_type(), target->GetHP(), damageAmount, TargetStaminaLoss, knockback_direction);

					// 타격 후 스테미너가 0미만이면 그로기로
					if (target->GetStamina() < 0) {
						if (target->ChangeToGroggyState(pServer, roomid)) {
							PushEventGroggyRecovery(pServer->GetTimer(), target_id, roomid, room->GetRoomCode(), target->GroggyRecoverTime());
						}
					}
				}

			}break;

			case eDamageType::AT_BOMB_ATTACK: {
				damageAmount += 20; // 임시

				// 타겟의 기력이 없으면 그로기 상태로
				if (target->GetStamina() == 0) {
					if (target->ChangeToGroggyState(pServer, roomid)) {
						PushEventGroggyRecovery(pServer->GetTimer(), target_id, roomid, room->GetRoomCode(), target->GroggyRecoverTime());
					}
				}

				// 데미지 적용
				target->ReduceHP(damageAmount);

				if (target->GetHP() <= 0) {
					// 사망 처리
					target->ChangeToDeadState(pServer, roomid);

					// record update
					room->GetPlayerRecordList()[target_id].death_count++;
					int spawnTime = tableManager->GetGameModeData()[room->GetGameMode()].Player_Spawn_Time;
					PushEventPlayerRespawn(pServer->GetTimer(), target_id, roomid, room->GetRoomCode(), spawnTime);
				}
				else {
					Vector3f knockback_direction(read->knockback_direction()->x(), read->knockback_direction()->y(), read->knockback_direction()->z());
					pServer->GetPacketSender()->SendPlayerCalculatedDamage(target_id, roomid, read->attack_type(), target->GetHP(), damageAmount, TargetStaminaLoss, knockback_direction);
				}

			}break;

			default: {
				room->GetPlayerListLock().unlock_shared();
				return;
			}break;
			}

			room->GetPlayerListLock().unlock_shared();
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};