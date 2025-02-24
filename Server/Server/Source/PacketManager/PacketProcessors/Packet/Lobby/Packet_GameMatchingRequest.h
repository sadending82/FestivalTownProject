#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::LobbyTable;

class Packet_GameMatchingRequest : public PacketProcessor {

public:
	Packet_GameMatchingRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (pServer->GetMode() == SERVER_MODE::LIVE && player->GetIsAuthenticated() == false) {
				pServer->Disconnect(key);
				return;
			}

			const GameMatchingRequest* read = flatbuffers::GetRoot<GameMatchingRequest>(data);

			SERVER_MODE serverMode = pServer->GetMode();

			eMatchingType matchingType = (eMatchingType)read->matching_type();

			MatchMakingManager* MatchMakingManager = pServer->GetMatchMakingManager();

			long long requestTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

			player->SetMatchingRequestType(matchingType);
			player->SetMatchingRequestTime(requestTime);

			if (player->ChangeSessionState(eSessionState::ST_ACCEPTED, eSessionState::ST_MATCHWAITING)) {

			}
			else {
				DEBUGMSGNOPARAM("Error!: Fail Change Session State ST_ACCEPTED To ST_MATCHWAITING");
				return;
			}

			switch (matchingType) {
			case eMatchingType::FITH_TUTORIAL: {

				int map_code = read->map_code();

				GameMode modeIndex = GameMode::FITH_Tutorial_1;
				MapProperties testMapProperties;

				if (map_code == 0) {
					modeIndex = GameMode::FITH_Tutorial_1;
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
							modeIndex = GameMode::FITH_Tutorial_1;
							testMapProperties = MatchMakingManager->SelectRandomMap(GameMode(modeIndex));
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
			default: {
				std::lock_guard<std::mutex> lock(MatchMakingManager->GetMatchingLock(matchingType));
				MatchMakingManager->GetMatchingQueue(matchingType).insert({ key, requestTime });

				std::wcout << L"Nickname: " << player->GetNickName() << L" Matching Request / Match: " << matchingType << L" / wating Player - " << MatchMakingManager->GetMatchingQueue(matchingType).size() << std::endl;;

			}break;
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_GameMatchingRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};