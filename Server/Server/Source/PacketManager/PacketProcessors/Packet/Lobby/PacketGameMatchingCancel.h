#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::LobbyTable;

class PacketGameMatchingCancel : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<GameMatchingCancel>(nullptr)) {

			const GameMatchingCancel* read = flatbuffers::GetRoot<GameMatchingCancel>(data);

			Session* session = pServer->GetSessions()[key];

			session->GetStateLock().lock();
			session->SetState(eSessionState::ST_ACCEPTED);
			session->GetStateLock().unlock();

			std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_MATCHING_CANCEL, data, size);
			session->DoSend(send_buffer.data(), send_buffer.size());
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};