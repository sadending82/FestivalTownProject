#pragma once
#include "../PacketProcessor.h"

using namespace PacketTable::GameTable;

class PacketBombInput : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<BombInput>(nullptr)) {
			const BombInput* read = flatbuffers::GetRoot<BombInput>(data);

			int team = read->team();
			int roomid = dynamic_cast<Player*>(pServer->GetSessions()[key])->GetRoomID();

			pServer->GetRooms()[roomid]->GetTeams()[team].ReduceLife();
		}
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};