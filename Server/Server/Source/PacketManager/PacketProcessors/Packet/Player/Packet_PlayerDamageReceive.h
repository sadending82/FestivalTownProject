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

			TableManager* tableManager = pServer->GetTableManager();
			CharacterStat& attackerStat = tableManager->GetCharacterStats()[(int)attacker->GetChacracterType()];

			target->GetPlayerStateLock().lock();
			int damageAmount = 0;

			// ������ ���
			switch (read->attack_type()) 
			{
			case eDamageType::AT_FALLDOWN: {
				damageAmount = 9999999;
				// �Ʒ� �ڵ���� ����ִ� ���¿��� ����Ǳ� ������ �׷α⿡�� ����ִ� ���·� �ٲ�
				if (target->GetPlayerState() == ePlayerState::PS_GROGGY) {
					target->SetPlayerState(ePlayerState::PS_ALIVE);
				}
			}break;

			case eDamageType::AT_BASIC_ATTACK: {
				if (target_id == attacker_id) {
					target->GetPlayerStateLock().unlock();
					room->GetPlayerListLock().unlock_shared();
					return;
				}

				damageAmount += attackerStat.strength;
			}break;

			case eDamageType::AT_WEAPON_ATTACK: {
				if (target_id == attacker_id) {
					target->GetPlayerStateLock().unlock();
					room->GetPlayerListLock().unlock_shared();
					return;
				}

				if (attacker->GetWeapon() == nullptr) {
					target->GetPlayerStateLock().unlock();
					room->GetPlayerListLock().unlock_shared();
					return;
				}

				damageAmount += attackerStat.strength + tableManager->GetWeaponStats()[(int)attacker->GetWeapon()->GetType()].Weapon_Power;
			}break;

			case eDamageType::AT_BOMB_ATTACK: {
				damageAmount += 20; // �ӽ�
			}break;

			default: {
				target->GetPlayerStateLock().unlock();
				room->GetPlayerListLock().unlock_shared();
				return;
			}break;
			}

			// ����ִ��� Ȯ��
			if (target->GetPlayerState() != ePlayerState::PS_ALIVE) {
				target->GetPlayerStateLock().unlock();
				room->GetPlayerListLock().unlock_shared();
				return;
			}
			target->GetPlayerStateLock().unlock();

			// Ÿ���� ����� ������ �׷α� ���·�
			if (target->GetStamina() == 0) {
				target->ChangeToGroggyState(pServer, roomid);
				PushEventGroggyRecovery(pServer->GetTimer(), target_id, roomid, room->GetRoomCode(), target->GroggyRecoverTime());
			}

			// ������ ����
			target->ReduceHP(damageAmount);

			if (target->GetHP() <= 0) {
				// ��� ó��
				target->ChangeToDeadState(pServer, roomid);

				// record update
				room->GetPlayerRecordList()[target_id].death_count++;
				if (read->attack_type() != AT_FALLDOWN) {
					room->GetPlayerRecordList()[attacker_id].kill_count++;
				}
				int spawnTime = tableManager->GetGameModeData()[room->GetGameMode()].Player_Spawn_Time;
				PushEventPlayerRespawn(pServer->GetTimer(), target_id, roomid, room->GetRoomCode(), spawnTime);
			}
			else {
				Vector3f knockback_direction(read->knockback_direction()->x(), read->knockback_direction()->y(), read->knockback_direction()->z());
				pServer->GetPacketSender()->SendPlayerCalculatedDamage(target_id, roomid, read->attack_type(), target->GetHP(), damageAmount, knockback_direction);
			}

			room->GetPlayerListLock().unlock_shared();
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};