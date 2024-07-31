#pragma once
#include "../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class PacketPlayerStop : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<PlayerStop>(nullptr)) {

			const PlayerStop* read = flatbuffers::GetRoot<PlayerStop>(data);

			Player* player = pServer->GetSessions()[key];
			player->SetPosition(read->pos()->x(), read->pos()->y(), read->pos()->z());
			player->SetDirection(read->direction()->x(), read->direction()->y(), read->direction()->z());

			std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYERSTOP, data, size);

			pServer->SendAllPlayerInRoomExceptSender(send_buffer.data(), send_buffer.size(), key);
		}
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};