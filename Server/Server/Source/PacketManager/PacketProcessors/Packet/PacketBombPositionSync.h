#pragma once
#include "../PacketProcessor.h"

using namespace PacketTable::ObjectTable;

class PacketBombPositionSync: public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		// ������ ���� ������ ��� X ��ȿ�� ���������� Ȯ���ؼ� ó��
		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<BombPosition>(nullptr)) {
			const BombPosition* read = flatbuffers::GetRoot<BombPosition>(data);

			int roomid = dynamic_cast<Player*>(pServer->GetSessions()[key])->GetInGameID();
			int bombid = read->id();

			Bomb* bomb = dynamic_cast<Bomb*>(pServer->GetRooms()[roomid]->GetObjects()[roomid]);

			bomb->SetPosition(Vector3f(read->pos()->x(), read->pos()->y(), read->pos()->z()));

			std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::C2S_BOMBPOSSYNC, data, size);
			pServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), key);
		}
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};