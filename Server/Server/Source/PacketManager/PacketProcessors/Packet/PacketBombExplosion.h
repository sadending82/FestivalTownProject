#pragma once
#include "../PacketProcessor.h"

using namespace PacketTable::ObjectTable;

class PacketBombExplosion : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<BombExplosion>(nullptr)) {
			const BombExplosion* read = flatbuffers::GetRoot<BombExplosion>(data);

			int roomid = dynamic_cast<Player*>(pServer->GetSessions()[key])->GetRoomID();
			int bombid = read->id();

			if (pServer->GetRooms()[roomid]->GetObjects()[bombid] == nullptr) {
				return;
			}

		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};