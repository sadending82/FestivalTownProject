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

            pServer->GetMatchingLock().lock();

            std::priority_queue<Player*
                , std::vector<Player*>
                , MatchingCompare> readyPlayers;

            for (Session* s : pServer->GetSessions()) {
                s->GetStateLock().lock();
                if (s->GetState() == eSessionState::ST_MATCHWAITING) {
                    readyPlayers.push(dynamic_cast<Player*>(s));
                }
                s->GetStateLock().unlock();
            }

            int waitingPlayerCount = readyPlayers.size();

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
                    if (readyPlayers.empty()) {
                        break;
                    }
                    Player* topPlayer = readyPlayers.top();

                    // 다음 매칭에 참고하기 위해 지금 매칭된 게임 종류 저장
                    if (gameMode == GameMode::FITH_Team_Battle_4 || gameMode == GameMode::FITH_Team_Battle_6) {
                        topPlayer->SetPlayedSoloGameBefore(false);
                    }
                    else {
                        topPlayer->SetPlayedSoloGameBefore(true);
                    }
                    playerList.push_back(topPlayer);
                    COUT << "Matched - " << topPlayer->GetSessionID() << ENDL;
                    readyPlayers.pop();
                }
                waitingPlayerCount = readyPlayers.size();
                pServer->MatchingComplete(roomid, playerList);
                std::cout << "Start Game room - " << roomid << "| GameMode - " << gameMode << std::endl;
            }

            pServer->GetMatchingLock().unlock();
            PushEventGameMatching(pServer->GetTimer());
        }
        catch (const std::exception& e) {
            std::cerr << "[ERROR] : " << e.what() << std::endl;
        }
	}

private:
};