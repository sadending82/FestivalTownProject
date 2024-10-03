#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerThrowBomb : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {
		try {

			mBuilder.Clear();
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<PlayerThrowBomb>(nullptr)) {
				const PlayerThrowBomb* read = flatbuffers::GetRoot<PlayerThrowBomb>(data);

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				if (player == nullptr && player->GetInGameID() != read->id()) {
					return;
				}
				player->GetBombLock().lock();
				Bomb* bomb = player->GetBomb();
				if (bomb == nullptr) {
					player->GetBombLock().unlock();
					return;
				}

				int roomid = player->GetRoomID();
				int bombid = read->bomb_id();
				Room* room = pServer->GetRooms().at(roomid);

				bomb->SetIsGrabbed(false);
				bomb->SetOwenrID(INVALIDKEY);
				player->SetBomb(nullptr);
				player->GetBombLock().unlock();

				std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYER_THROW_BOMB, data, size);
				pServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomid);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};