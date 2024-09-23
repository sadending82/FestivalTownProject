#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class PacketPlayerDamageReceive : public PacketProcessor {

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

			// 팀킬 예외처리
			if (target->GetTeam() == attacker->GetTeam()) {
				room->GetPlayerListLock().unlock_shared();
				return;
			}

			// 살아있는지 확인
			target->GetPlayerStateLock().lock();
			if (target->GetPlayerState() != ePlayerState::PS_ALIVE) {
				target->GetPlayerStateLock().unlock();
				room->GetPlayerListLock().unlock_shared();
				return;
			}

			// 타겟의 기력이 없으면 그로기 상태로
			if (target->GetStamina() == 0) {
				target->SetPlayerState(ePlayerState::PS_GROGGY);
				target->AddGroggyCount();
				pServer->GetPacketSender()->SendPlayerGroggyPacket(read->target_id(), roomid);

				// 들고있는 무기 해제
				Weapon* weapon = target->GetWeapon();
				if (weapon != nullptr) {
					if (weapon->SetIsGrabbed(false) == true) {
						int weaponID = weapon->GetID();
						weapon->SetOwenrID(INVALIDKEY);
						target->SetWeapon(nullptr);
						weapon->SetPosition(target->GetPosition());
						pServer->GetPacketSender()->SendWeaponDropPacket(target->GetPosition(), roomid, weaponID);
					}
				}

				PushEventGroggyRecovery(pServer->GetTimer(), target_id, roomid, room->GetRoomCode(), target->GroggyRecoverTime());
			}

			TableManager* tableManager = pServer->GetTableManager();
			CharacterStat& attackerStat = tableManager->GetCharacterStats()[(int)attacker->GetChacracterType()];
			WeaponStat& attackerWeaponStat = tableManager->GetWeaponStats()[(int)attacker->GetWeapon()->GetType()];

			// 데미지 계산
			int damageAmount = 0;
			if (read->attack_type() == AT_FALLDOWN) {
				// 낙사 시 즉사
				damageAmount = 9999999;
			}
			else {
				damageAmount += attackerStat.strength; // 임시
				if (attacker->GetWeapon() != nullptr) {
					damageAmount += attackerWeaponStat.Weapon_Power;
				}
			}

			target->ReduceHP(damageAmount);

			if (target->GetHP() <= 0) {
				// 사망 처리
				target->SetPlayerState(ePlayerState::PS_DEAD);
				int spawnTime = tableManager->GetGameModeData()[room->GetGameMode()].Player_Spawn_Time;
				pServer->GetPacketSender()->SendPlayerDeadPacket(read->target_id(), roomid);

				if (target->GetWeapon() != nullptr) {
					if (target->GetWeapon()->SetIsGrabbed(false) == true) {
						int weaponID = target->GetWeapon()->GetID();
						target->GetWeapon()->SetOwenrID(INVALIDKEY);
						target->GetWeapon()->SetPosition(target->GetPosition());
						target->SetWeapon(nullptr);
						pServer->GetPacketSender()->SendWeaponDropPacket(target->GetPosition(), roomid, weaponID);
					}
				}

				// record update
				room->GetPlayerRecordList()[target_id].death_count++;
				if (read->attack_type() != AT_FALLDOWN) {
					room->GetPlayerRecordList()[attacker_id].kill_count++;
				}
				PushEventPlayerRespawn(pServer->GetTimer(), target_id, roomid, room->GetRoomCode(), spawnTime);
			}
			else {
				Vector3f knockback_direction(read->knockback_direction()->x(), read->knockback_direction()->y(), read->knockback_direction()->z());
				pServer->GetPacketSender()->SendPlayerCalculatedDamage(target_id, roomid, read->attack_type(), target->GetHP(), damageAmount, knockback_direction);
			}
			target->GetPlayerStateLock().unlock();

			room->GetPlayerListLock().unlock_shared();
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};