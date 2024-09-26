#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::LobbyTable;

class Packet_GameMatchingRequest : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<GameMatchingRequest>(nullptr)) {

			const GameMatchingRequest* read = flatbuffers::GetRoot<GameMatchingRequest>(data);

			Session* session = pServer->GetSessions()[key];

			SERVER_MODE serverMode = pServer->GetMode();

			session->SetMatchingRequestTime(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
			session->GetStateLock().lock();
			session->SetState(eSessionState::ST_MATCHWAITING);
			session->GetStateLock().unlock();

			switch (serverMode) {
			case SERVER_MODE::LIVE: {

			}break;
			case SERVER_MODE::TEST: {
				std::vector<Player*> playerVector = {dynamic_cast<Player*>(session)};
				pServer->MatchingComplete(TESTROOM, playerVector);
			}break;
			}
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};