#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerSync : public PacketProcessor {

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

			int roomid = player->GetRoomID();
			Room* room = pServer->GetRooms()[roomid];

			player->SetPosition(read->pos()->x(), read->pos()->y(), read->pos()->z());
			player->SetDirection(read->direction()->x(), read->direction()->y(), read->direction()->z());
			player->SetStamina(read->stamina());

			if (pServer->GetGameManagers()[room->GetGameMode()]->CheckValidPlayerPosition(player->GetPosition()) == false) {
				int spawnTime = pServer->GetTableManager()->GetGameModeData()[room->GetGameMode()].Player_Spawn_Time;
				if (player->ChangeToDeadState(pServer, spawnTime)) {
					// record update
					int roomid = player->GetRoomID();
					Room* room = pServer->GetRooms()[roomid];
					room->GetPlayerRecordList()[playerid].death_count++;
					PushEventPlayerRespawn(pServer->GetTimer(), playerid, roomid, room->GetRoomCode(), spawnTime);
				}
				return;
			}

			if (player->GetIsGrabbed() == true) {
				return;
			}

			std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYER_SYNC, data, size);

			pServer->SendAllPlayerInRoomExceptSender(send_buffer.data(), send_buffer.size(), key);
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};