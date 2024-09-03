#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::LobbyTable;

class PacketGameMatchingRequest : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<GameMatchingRequest>(nullptr)) {

			const GameMatchingRequest* read = flatbuffers::GetRoot<GameMatchingRequest>(data);

			Session* session = pServer->GetSessions()[key];

			session->SetMatchingRequestTime(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());

			session->GetStateLock().lock();
			session->SetState(eSessionState::ST_GAMEREADY);
			session->GetStateLock().unlock();
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};