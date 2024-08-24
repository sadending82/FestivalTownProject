#pragma once
#include "../PacketProcessor.h"

using namespace PacketTable::ObjectTable;

class PacketBombPositionSync: public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		// 지금은 버퍼 내용은 사용 X 유효한 버퍼인지만 확인해서 처리
		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<BombPosition>(nullptr)) {
			const BombPosition* read = flatbuffers::GetRoot<BombPosition>(data);

			Player* p = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			int roomid = p->GetRoomID();
			int bombid = read->id();

			Room* room = pServer->GetRooms()[roomid];
			room->GetObjectListLock().lock_shared();
			Bomb* bomb = dynamic_cast<Bomb*>(room->GetObjects()[bombid]);
			if (bomb == nullptr) {
				room->GetObjectListLock().unlock_shared();
				return;
			}

			bomb->SetPosition(Vector3f(read->pos()->x(), read->pos()->y(), read->pos()->z()));

			std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_BOMBPOSSYNC, data, size);
			pServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomid);
			room->GetObjectListLock().unlock_shared();
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};