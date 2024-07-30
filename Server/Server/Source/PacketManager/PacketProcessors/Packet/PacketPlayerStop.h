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

			std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYERSTOP, data, size);

			int roomID = reinterpret_cast<Player*>(pServer->GetSessions()[key])->GetRoomID();
			for (Player* p : pServer->GetRooms()[roomID]->GetPlayerList()) {
				if (p == nullptr) continue;
				if (p->GetSessionID() == key)
					p->DoSend(send_buffer.data(), send_buffer.size());
			}
		}
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};