#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerGrabBomb : public PacketProcessor {

public:
	Packet_PlayerGrabBomb(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<PlayerGrabBomb>(nullptr)) {
				const PlayerGrabBomb* read = flatbuffers::GetRoot<PlayerGrabBomb>(data);
				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				int playerid = player->GetInGameID();
				if (player == nullptr && playerid != read->id()) {
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

				player->GetWeaponLock().lock();
				if (player->GetWeapon() != nullptr) {
					player->GetWeaponLock().unlock();
					return;
				}
				player->GetWeaponLock().unlock();

				player->GetBombLock().lock();
				if (player->GetBomb() != nullptr) {
					player->GetBombLock().unlock();
					return;
				}
				int bombid = read->bomb_id();

				Bomb* bomb = room->GetBomb(bombid);
				if (bomb == nullptr) {
					player->GetBombLock().unlock();
					return;
				}
				if (bomb->SetIsGrabbed(true) == true) {
					bomb->SetOwenrID(playerid);
					player->SetBomb(bomb);
					room->GetPlayerRecordList()[playerid].gameRecord.Pick_Bomb_Count.fetch_add(1);
					std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYER_GRAB_BOMB, data, size);
					pServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
				}
				else {
					player->SetBomb(nullptr);
				}
				player->GetBombLock().unlock();
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_PlayerGrabBomb ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};