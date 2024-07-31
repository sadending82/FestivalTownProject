#pragma once
#include "../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class PacketPlayerPosSync : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<PlayerPos>(nullptr)) {

			const PlayerPos* read = flatbuffers::GetRoot<PlayerPos>(data);

			Player* player = reinterpret_cast<Player*>(pServer->GetSessions()[key]);
			player->SetPosition(read->pos()->x(), read->pos()->y(), read->pos()->z());
			player->SetDirection(read->direction()->x(), read->direction()->y(), read->direction()->z());

			std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYERPOSSYNC, data, size);

			pServer->SendAllPlayerInRoomExceptSender(send_buffer.data(), send_buffer.size(), key);
		}
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};