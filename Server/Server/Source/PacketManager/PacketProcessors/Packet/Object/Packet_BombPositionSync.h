#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::ObjectTable;

class Packet_BombPositionSync: public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		// ������ ���� ������ ��� X ��ȿ�� ���������� Ȯ���ؼ� ó��
		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<BombPosition>(nullptr)) {
			const BombPosition* read = flatbuffers::GetRoot<BombPosition>(data);

			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (player == nullptr) {
				return;
			}
			int roomid = player->GetRoomID();
			int bombid = read->id();

			Room* room = pServer->GetRooms()[roomid];
			room->GetBombListLock().lock_shared();
			Bomb* bomb = dynamic_cast<Bomb*>(room->GetBombList()[bombid]);
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

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};