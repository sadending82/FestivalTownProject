#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class Packet_PlayerGrabOtherPlayer : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();
		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<PlayerGrabOtherPlayer>(nullptr)) {
			const PlayerGrabOtherPlayer* read = flatbuffers::GetRoot<PlayerGrabOtherPlayer >(data);

			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (player == nullptr && player->GetInGameID() != read->id()) {
				return;
			}
			
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};