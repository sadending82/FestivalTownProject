#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerThrowBomb : public PacketProcessor {

public:
	Packet_PlayerThrowBomb(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {

			mBuilder.Clear();
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<PlayerThrowBomb>(nullptr)) {
				const PlayerThrowBomb* read = flatbuffers::GetRoot<PlayerThrowBomb>(data);

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				if (player == nullptr && player->GetInGameID() != read->id()) {
					return;
				}
				int roomid = player->GetRoomID();
				if (roomid == INVALIDKEY) {
					return;
				}
				Room* room = pServer->GetRooms().at(roomid);
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

				bomb->SetIsGrabbed(false);
				bomb->SetOwenrID(INVALIDKEY);
				player->SetBomb(nullptr);
				player->GetBombLock().unlock();

				std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYER_THROW_BOMB, data, size);
				pServer->SendAllPlayerInRoomExceptSender(send_buffer.data(), send_buffer.size(), key);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_PlayerThrowBomb ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};