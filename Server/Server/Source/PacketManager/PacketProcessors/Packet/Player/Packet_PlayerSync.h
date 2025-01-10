#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerSync : public PacketProcessor {

public:
	Packet_PlayerSync(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<PlayerSync>(nullptr)) {

				const PlayerSync* read = flatbuffers::GetRoot<PlayerSync>(data);
				int playerid = read->id();

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				if (player == nullptr && player->GetInGameID() != read->id()) {
					return;
				}

				int roomID = player->GetRoomID();
				if (roomID == INVALIDKEY) {
					return;
				}
				Room* room = pServer->GetRooms().at(roomID);
				if (room == nullptr && (room->GetState() != eRoomState::RS_INGAME)) {
					return;
				}

				player->SetPosition(read->pos()->x(), read->pos()->y(), read->pos()->z());
				player->SetDirection(read->direction()->x(), read->direction()->y(), read->direction()->z());
				player->SetStamina(read->stamina());

				//if (pServer->GetGameManagers()[room->GetGameMode()]->CheckValidPlayerPosition(player->GetPosition()) == false) {
				//	int spawnTime = room->GetGameModeData().Player_Spawn_Time;
				//	if (player->ChangeToDeadState(pServer, spawnTime)) {
				//		// record update
				//		int roomID = player->GetRoomID();
				//		Room* room = pServer->GetRooms().at(roomID);
				//		room->GetPlayerRecordList().at(playerid).gameRecord.DeathCount.fetch_add(1);
				//		PushEventPlayerRespawn(pServer->GetTimer(), playerid, roomID, room->GetRoomCode(), spawnTime);
				//	}
				//	return;
				//}

				if (player->GetIsGrabbed() == true) {
					return;
				}

				std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYER_SYNC, data, size);

				pServer->SendAllPlayerInRoomExceptSender(send_buffer.data(), send_buffer.size(), key);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_PlayerSync ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};