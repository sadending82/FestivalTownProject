#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"
#include <queue>

class Event_GameMatching : public PacketProcessor {

public:
    Event_GameMatching(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(unsigned char* buf) { 
        try {
            EV_GAME_MATCHING* event = reinterpret_cast<EV_GAME_MATCHING*>(buf);

            MatchMakingManager* MatchMakingManager = pServer->GetMatchMakingManager();

            MatchMakingManager->GetMatchingLock().lock();

            MATCHING_QUEUE& matchingQueue = MatchMakingManager->GetMatchingQueue(eMatchingType::FITH_TEAM);

            int waitingPlayerCount = matchingQueue.size();

            TableManager* tableManager = pServer->GetTableManager();
            GAMEMANAGER_MAP& gameManagers = pServer->GetGameManagers();

            while (waitingPlayerCount >= MINPLAYER) {
                GameMode gameMode = CulculateGameMode(waitingPlayerCount);

                int matchedPlayerCount = tableManager->GetGameModeData()[gameMode].Player_Count;

                int roomid = pServer->CreateNewRoom(gameMode);
                if (roomid == INVALIDKEY) {
                    std::cout << "Fali Create New Room\n";
                    break;
                }

                std::vector<Player*> playerList;
                for (int i = 0; i < matchedPlayerCount; ++i) {
                    if (matchingQueue.empty()) {
                        break;
                    }
                    Player* topPlayer = *matchingQueue.begin();

                    playerList.push_back(topPlayer);
                    matchingQueue.erase(matchingQueue.begin());
                }
                waitingPlayerCount = matchingQueue.size();
                MatchMakingManager->MatchingComplete(roomid, playerList);
                std::cout << "Start Game room - " << roomid << "| GameMode - " << gameMode << std::endl;
            }

            MatchMakingManager->GetMatchingLock().unlock();
            PushEventGameMatching(pServer->GetTimer());
        }
        catch (const std::exception& e) {
            std::cerr << "[ERROR] : " << e.what() << std::endl;
        }
	}

private:
};