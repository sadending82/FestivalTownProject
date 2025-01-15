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
				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				if (pServer->GetMode() == SERVER_MODE::LIVE) {
					if (player->GetIsAuthenticated() == false) {
						pServer->Disconnect(key);
						return;
					}
				}

				const GameMatchingRequest* read = flatbuffers::GetRoot<GameMatchingRequest>(data);

				SERVER_MODE serverMode = pServer->GetMode();

				eMatchingType matchingType = (eMatchingType)read->matching_type();

				MatchMakingManager* MatchMakingManager = pServer->GetMatchMakingManager();

				long long requestTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

				player->SetMatchingRequestType(matchingType);
				player->SetMatchingRequestTime(requestTime);

				player->GetSessionStateLock().lock();
				player->SetSessionState(eSessionState::ST_MATCHWAITING);
				player->GetSessionStateLock().unlock();


				MatchMakingManager->GetMatchingLock().lock();
				switch (matchingType) {
				case eMatchingType::FITH_TEST: {

					if (serverMode != SERVER_MODE::TEST) {
						break;
					}

					int map_code = read->map_code();

					GameMode modeIndex = GameMode::INVALUE_MODE;
					MapProperties testMapProperties;

					if (map_code == 0) {
						std::vector<int> modeList = { 111, 112, 121, 122 };

						std::random_device rd;
						std::mt19937 gen(rd());
						std::uniform_int_distribution<>map_distrib(0, modeList.size() - 1);

						modeIndex = (GameMode)modeList[map_distrib(rd)];
						testMapProperties = MatchMakingManager->SelectRandomMap(GameMode(modeIndex));
					}
					else {
						for (auto pair : pServer->GetTableManager()->getMapListByMode()) {
							for (auto m : pair.second) {
								if (m == map_code) {
									modeIndex = pair.first;
									break;
								}
							}
							if (modeIndex != GameMode::INVALUE_MODE) {
								break;
							}
						}

						testMapProperties.Map_Index = map_code;
						testMapProperties.Map_Theme = 1;
					}

					int roomID = pServer->CreateNewRoom((GameMode)modeIndex, testMapProperties.Map_Index, testMapProperties.Map_Theme);

					if (roomID == INVALIDKEY) {
						break;
					}

					Room* room = pServer->GetRooms()[roomID];
					room->SetIsTestRoom(true);


					// ¸Ê Å×½ºÆ® ¶«¿¡ Ãß°¡
					room->InitMap(pServer->GetTableManager()->GetMapData()[testMapProperties.Map_Index], testMapProperties.Map_Theme);

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

					//std::cout << "MAP: " << testMapProperties.Map_Index << " THEME: "<< testMapProperties.Map_Theme << std::endl;;
				}break;
				default:{
					MatchMakingManager->GetMatchingQueue(matchingType).insert({key, requestTime});

					std::wcout << L"Nickname: " << player->GetNickName() << L" Matching Request / Match: " << matchingType << L" / wating Player - " << MatchMakingManager->GetMatchingQueue(matchingType).size() << std::endl;;

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