#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::GameTable;

class Packet_GameReady : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();
		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<GameReady>(nullptr)) {

			const GameReady* read = flatbuffers::GetRoot<GameReady>(data);
			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (player == nullptr) {
				return;
			}
			int roomid = player->GetRoomID();
			Room* room = pServer->GetRooms()[roomid];
			if (room == nullptr) {
				return;
			}

			room->AddReadyCnt();

			SERVER_MODE serverMode = pServer->GetMode();
			switch (serverMode) {
			case SERVER_MODE::LIVE: {
				if (room->GetReadyCnt() == room->GetPlayerCnt()) {

					if (room->SetAllPlayerReady(true) == true) {

						pServer->GetPacketSender()->SendAllPlayerReady(roomid);
						PushEventGameStart(pServer->GetTimer(), roomid, room->GetRoomCode());
					}
				}
			}break;
			case SERVER_MODE::TEST: {
				pServer->GetPacketSender()->SendAllPlayerReady(TESTROOM);
				PushEventGameStart(pServer->GetTimer(), TESTROOM, pServer->GetRooms()[TESTROOM]->GetRoomCode());
			}break;
			}
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};