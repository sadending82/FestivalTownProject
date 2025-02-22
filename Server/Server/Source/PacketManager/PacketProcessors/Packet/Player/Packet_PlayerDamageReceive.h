#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerDamageReceive : public PacketProcessor {

public:
	Packet_PlayerDamageReceive(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) { 
		try {
			const PlayerDamageReceive* read = flatbuffers::GetRoot<PlayerDamageReceive>(data);

			int attackerID = read->attacker_id();
			int targetID = read->target_id();
			int roomID = dynamic_cast<Player*>(pServer->GetSessions()[key])->GetRoomID();
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

			Vector3f knockback_direction(read->knockback_direction()->x(), read->knockback_direction()->y(), read->knockback_direction()->z());
			GameManager* gameManager = pServer->GetGameManagers().at(room->GetGameMode());

			gameManager->PlayerDamageReceive(roomID, room, attackerID, attacker, targetID, target, knockback_direction, (eDamageType)read->attack_type());
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_PlayerDamageReceive ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};