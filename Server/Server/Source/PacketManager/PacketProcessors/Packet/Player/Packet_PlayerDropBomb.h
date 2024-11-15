#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerDropBomb : public PacketProcessor {

public:
	Packet_PlayerDropBomb(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<PlayerDropBomb>(nullptr)) {
				const PlayerDropBomb* read = flatbuffers::GetRoot<PlayerDropBomb>(data);
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

				player->GetBombLock().lock();
				Bomb* bomb = player->GetBomb();
				if (bomb == nullptr) {
					player->GetBombLock().unlock();
					return;
				}

				int bombid = read->bomb_id();

				bomb->SetPosition(player->GetPosition());
				bomb->SetIsGrabbed(false);
				bomb->SetOwenrID(INVALIDKEY);
				player->SetBomb(nullptr);
				player->GetBombLock().unlock();

				std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYER_DROP_BOMB, data, size);
				pServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_PlayerDropBomb ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};