#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::GameTable;

class PacketBombInput : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<BombInput>(nullptr)) {
			const BombInput* read = flatbuffers::GetRoot<BombInput>(data);

			int playerid = read->id();
			int bombid = read->bombid();
			int team = read->team();
			int roomid = dynamic_cast<Player*>(pServer->GetSessions()[key])->GetRoomID();
			Room* room = pServer->GetRooms()[roomid];

			room->DeleteObject(bombid);

			pServer->GetRooms()[roomid]->GetTeams()[team].ReduceLife();
			int lifeCount = pServer->GetRooms()[roomid]->GetTeams()[team].GetLife();
			pServer->SendLifeReducePacket(team, lifeCount, roomid);

			if (lifeCount <= 0) {
				pServer->CheckGameEnd(roomid);
			}
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};