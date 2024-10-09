#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::ObjectTable;

class Packet_BombPositionSync: public PacketProcessor {

public:
	Packet_BombPositionSync(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {

		try {
			mBuilder.Clear();

			// 지금은 버퍼 내용은 사용 X 유효한 버퍼인지만 확인해서 처리
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<BombPosition>(nullptr)) {
				const BombPosition* read = flatbuffers::GetRoot<BombPosition>(data);

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				if (player == nullptr) {
					return;
				}
				int roomid = player->GetRoomID();
				if (roomid == INVALIDKEY) {
					return;
				}
				int bombid = read->id();

				Room* room = pServer->GetRooms().at(roomid);
				room->GetBombListLock().lock_shared();
				Bomb* bomb = dynamic_cast<Bomb*>(room->GetBombList().at(bombid));
				if (bomb == nullptr) {
					room->GetBombListLock().unlock_shared();
					return;
				}

				bomb->SetPosition(Vector3f(read->pos()->x(), read->pos()->y(), read->pos()->z()));

				std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_BOMB_POS_SYNC, data, size);
				pServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomid);
				room->GetBombListLock().unlock_shared();
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_BombPositionSync ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};