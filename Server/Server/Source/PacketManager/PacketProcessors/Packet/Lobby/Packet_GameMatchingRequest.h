#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::LobbyTable;

class Packet_GameMatchingRequest : public PacketProcessor {

public:
	Packet_GameMatchingRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			

			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<GameMatchingRequest>(nullptr)) {

				const GameMatchingRequest* read = flatbuffers::GetRoot<GameMatchingRequest>(data);

				Session* session = pServer->GetSessions()[key];

				SERVER_MODE serverMode = pServer->GetMode();

				session->SetMatchingRequestTime(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
				session->GetSessionStateLock().lock();
				session->SetSessionState(eSessionState::ST_MATCHWAITING);
				session->GetSessionStateLock().unlock();

				switch (serverMode) {
				case SERVER_MODE::LIVE: {

				}break;
				case SERVER_MODE::TEST: {
					/*Room* room = pServer->GetRooms()[TESTROOM];
					room->GetStateLock().lock();
					if (room->GetState() == eRoomState::RS_FREE) {
						pServer->MakeTestRoom();
					}
					room->GetStateLock().unlock();*/

					int roomID = pServer->CreateNewRoom(GameMode::FITH_Indiv_Battle_2);

					int botID = pServer->SetSessionID();

					if (botID == INVALIDKEY) {
						return;
					}

					Player* Bot = dynamic_cast<Player*>(pServer->GetSessions()[botID]);
					Bot->SetSessionState(eSessionState::ST_MATCHWAITING);
					Bot->SetIsBot(true);

					std::vector<Player*> playerVector = { dynamic_cast<Player*>(session), Bot };
					pServer->MatchingComplete(roomID, playerVector);
				}break;
				}
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_GameMatchingRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};