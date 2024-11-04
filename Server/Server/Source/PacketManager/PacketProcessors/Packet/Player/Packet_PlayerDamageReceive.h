#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerDamageReceive : public PacketProcessor {

public:
	Packet_PlayerDamageReceive(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) { 
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<PlayerDamageReceive>(nullptr)) {

				const PlayerDamageReceive* read = flatbuffers::GetRoot<PlayerDamageReceive>(data);

				int attacker_id = read->attacker_id();
				int target_id = read->target_id();
				int roomid = dynamic_cast<Player*>(pServer->GetSessions()[key])->GetRoomID();
				if (roomid == INVALIDKEY) {
					return;
				}
				Room* room = pServer->GetRooms().at(roomid);

				room->GetStateLock().lock_shared();
				if (room->GetState() != eRoomState::RS_INGAME) {
					room->GetStateLock().unlock_shared();
					return;
				}
				room->GetStateLock().unlock_shared();

				int target_sessionID = room->GetPlayerList()[read->target_id()].load();
				if (target_sessionID == INVALIDKEY) {
					return;
				}
				int attacker_sessionID = room->GetPlayerList()[read->attacker_id()].load();
				if (attacker_sessionID == INVALIDKEY) {
					return;
				}

				Player* target = dynamic_cast<Player*>(pServer->GetSessions()[target_sessionID]);
				Player* attacker = dynamic_cast<Player*>(pServer->GetSessions()[attacker_sessionID]);

				if (target == nullptr || attacker == nullptr) {
					return;
				}

				TableManager* tableManager = pServer->GetTableManager();
				CharacterStat& attackerStat = attacker->GetCharacterStat();

				int damageAmount = 0;
				int TargetStaminaLoss = 0;

				switch (read->attack_type())
				{
				case eDamageType::AT_FALLDOWN: {
					damageAmount = 9999999;
					// ��� ó��
					target->ReduceHP(damageAmount);
					int spawnTime = room->GetGameModeData().Player_Spawn_Time;
					if (target->ChangeToDeadState(pServer, spawnTime)) {
						// record update
						room->GetPlayerRecordList()[target_id].death_count++;
						PushEventPlayerRespawn(pServer->GetTimer(), target_id, roomid, room->GetRoomCode(), spawnTime);
					}
				}break;

				case eDamageType::AT_ATTACK: {
					if (target_id == attacker_id) {
						return;
					}
					// ��ų ����
					if (target->GetTeam() == attacker->GetTeam()) {
						return;
					}

					target->GetPlayerStateLock().lock();
					// ����ִ��� Ȯ��
					if (target->GetPlayerState() != ePlayerState::PS_ALIVE) {
						target->GetPlayerStateLock().unlock();
						return;
					}
					target->GetPlayerStateLock().unlock();


					// Ÿ���� ����� ������ �׷α� ���·�
					if (target->GetStamina() == 0) {
						if (target->ChangeToGroggyState(pServer)) {
							PushEventGroggyRecovery(pServer->GetTimer(), target_id, roomid, room->GetRoomCode(), room->GetGameModeData().Ch_Groggy);
						}
					}

					// ������ ���
					damageAmount += attackerStat.strength * attackerStat.attackStats[eDamageType::AT_ATTACK].Value;
					attacker->GetWeaponLock().lock_shared();
					if (attacker->GetWeapon() != nullptr) {
						damageAmount = attackerStat.strength + attacker->GetWeapon()->GetStat().Weapon_Power;
					}
					attacker->GetWeaponLock().unlock_shared();
					TargetStaminaLoss += attackerStat.attackStats[eDamageType::AT_ATTACK].Vanish_Stamina;

					// ������ ����
					target->ReduceHP(damageAmount);
					target->ReduceStamina(TargetStaminaLoss);


					if (target->GetHP() <= 0) {
						// ��� ó��
						int spawnTime = room->GetGameModeData().Player_Spawn_Time;
						target->ChangeToDeadState(pServer, spawnTime);

						// record update
						room->GetPlayerRecordList().at(target_id).death_count++;
						room->GetPlayerRecordList().at(attacker_id).kill_count++;
						PushEventPlayerRespawn(pServer->GetTimer(), target_id, roomid, room->GetRoomCode(), spawnTime);
					}
					else {
						Vector3f knockback_direction(read->knockback_direction()->x(), read->knockback_direction()->y(), read->knockback_direction()->z());
						pPacketSender->SendPlayerCalculatedDamage(target_id, roomid, read->attack_type(), target->GetHP(), damageAmount, TargetStaminaLoss, knockback_direction);

						// Ÿ�� �� ���׹̳ʰ� 0�̸��̸� �׷α��
						if (target->GetStamina() < 0) {
							if (target->ChangeToGroggyState(pServer)) {
								PushEventGroggyRecovery(pServer->GetTimer(), target_id, roomid, room->GetRoomCode(), room->GetGameModeData().Ch_Groggy);
							}
						}
					}

				}break;

				case eDamageType::AT_BOMB_ATTACK: {

					target->GetPlayerStateLock().lock();
					// ����ִ��� Ȯ��
					if (target->GetPlayerState() != ePlayerState::PS_ALIVE) {
						target->GetPlayerStateLock().unlock();
						return;
					}
					target->GetPlayerStateLock().unlock();


					damageAmount += room->GetGameModeData().Bomb_Damage;

					// Ÿ���� ����� ������ �׷α� ���·�
					if (target->GetStamina() == 0) {
						if (target->ChangeToGroggyState(pServer)) {
							PushEventGroggyRecovery(pServer->GetTimer(), target_id, roomid, room->GetRoomCode(), room->GetGameModeData().Ch_Groggy);
						}
					}

					// ������ ����
					target->ReduceHP(damageAmount);

					if (target->GetHP() <= 0) {
						// ��� ó��
						int spawnTime = room->GetGameModeData().Player_Spawn_Time;
						target->ChangeToDeadState(pServer, spawnTime);

						// record update
						room->GetPlayerRecordList()[target_id].death_count++;
						PushEventPlayerRespawn(pServer->GetTimer(), target_id, roomid, room->GetRoomCode(), spawnTime);
					}
					else {
						Vector3f knockback_direction(read->knockback_direction()->x(), read->knockback_direction()->y(), read->knockback_direction()->z());
						pPacketSender->SendPlayerCalculatedDamage(target_id, roomid, read->attack_type(), target->GetHP(), damageAmount, TargetStaminaLoss, knockback_direction);
					}

				}break;

				default: {
					return;
				}break;
				}
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_PlayerDamageReceive ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};