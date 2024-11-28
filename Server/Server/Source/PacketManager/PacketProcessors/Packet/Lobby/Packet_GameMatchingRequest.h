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

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);

				SERVER_MODE serverMode = pServer->GetMode();

				eMatchingType matchingType = (eMatchingType)read->matching_type();

				MatchMakingManager* MatchMakingManager = pServer->GetMatchMakingManager();

				MatchMakingManager->GetMatchingLock().lock();

				long long requestTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

				player->SetMatchingRequestType(matchingType);
				player->SetMatchingRequestTime(requestTime);

				player->GetSessionStateLock().lock();
				player->SetSessionState(eSessionState::ST_MATCHWAITING);
				player->GetSessionStateLock().unlock();

				switch (matchingType) {
				case eMatchingType::FITH_TEST: {

					if (serverMode != SERVER_MODE::TEST) {
						break;
					}
					int testMap = 11101;
					int modeIndex = 111;

					int roomID = pServer->CreateNewRoom((GameMode)modeIndex, testMap, 1);

					if (roomID == INVALIDKEY) {
						break;
					}

					Room* room = pServer->GetRooms()[roomID];
					room->SetIsTestRoom(true);


					// ¸Ê Å×½ºÆ® ¶«¿¡ Ãß°¡
					room->InitMap(&pServer->GetTableManager()->GetMapData()[testMap], 1);

					int botID = pServer->SetSessionID();

					if (botID == INVALIDKEY) {
						MatchMakingManager->GetMatchingLock().unlock();
						return;
					}

					Player* Bot = dynamic_cast<Player*>(pServer->GetSessions()[botID]);
					Bot->SetSessionState(eSessionState::ST_MATCHWAITING);
					Bot->SetIsBot(true);
					Bot->SetUID(INVALIDKEY);

					std::vector<int> sessions = { key, Bot->GetSessionID() };
					pServer->GetMatchMakingManager()->MatchingComplete(roomID, sessions);
				}break;
				default:{

					int matchingSequence = MatchMakingManager->GetMatchingSequence(matchingType);

					MatchMakingManager->GetMatchingQueue(matchingType).insert({key, requestTime});

					if (matchingSequence == eMatchingSequence::MS_None) {
						
					}
					else {
						MatchMakingManager->CheckMatchMaking(matchingType);
					}
					MatchMakingManager->UpdateMatchingSequence(matchingType);

				}break;
				}
				MatchMakingManager->GetMatchingLock().unlock();
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_GameMatchingRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};