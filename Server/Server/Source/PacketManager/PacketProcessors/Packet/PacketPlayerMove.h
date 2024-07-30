#pragma once
#include "../PacketProcessor.h"

using namespace PacketTable::PlayerTable;

class PacketPlayerMove : public PacketProcessor {

public:

	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<PlayerMove>(nullptr)) {

			const PlayerMove* read = flatbuffers::GetRoot<PlayerMove>(data);

			std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYERMOVE, data, size);
			
			int roomID = reinterpret_cast<Player*>(pServer->GetSessions()[key])->GetRoomID();
			for (Player* p : pServer->GetRooms()[roomID]->GetPlayerList()) {
				if (p == nullptr) continue;
				if (p->GetSessionID() == key) continue;
				p->DoSend(send_buffer.data(), send_buffer.size());
			}
		}
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};