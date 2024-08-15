#pragma once
#include "../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class PacketPlayerGrabBomb : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();
		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<PlayerGrabBomb>(nullptr)) {
			const PlayerGrabBomb* read = flatbuffers::GetRoot<PlayerGrabBomb>(data);

			int roomid = dynamic_cast<Player*>(pServer->GetSessions()[read->id()])->GetInGameID();
			int bombid = read->bomb_id();

			Bomb* bomb = dynamic_cast<Bomb*>(pServer->GetRooms()[roomid]->GetObjects()[roomid]);

			bomb->SetIsGrabbed(true);

			std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::C2S_PLAYERGRABBOMB, data, size);
			pServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), key);
		}
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};