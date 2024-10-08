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

            std::priority_queue<std::pair<int, Player*>
                , std::vector<std::pair<int, Player*>>
                , std::greater<std::pair<int, Player*>>> readyPlayers;

            for (Session* s : pServer->GetSessions()) {
                s->GetStateLock().lock();
                if (s->GetState() == eSessionState::ST_MATCHWAITING) {
                    readyPlayers.push({ s->GetMatchingRequestTime(), dynamic_cast<Player*>(s) });
                }
                s->GetStateLock().unlock();
            }

            int waitingPlayerCount = readyPlayers.size();
            std::unordered_map<GameMode, GameModeData>& GameModeDatas = pServer->GetTableManager()->GetGameModeData();

            while (waitingPlayerCount >= MINPLAYER) {
                GameMode gameMode = GameMode::FITH_Team_Battle_6;

                for (auto iter = GameModeDatas.begin(); iter != GameModeDatas.end(); iter++) {
                    if (waitingPlayerCount == iter->second.Player_Count) {
                        gameMode = iter->first;
                        break;
                    }
                }

                int roomid = pServer->CreateNewRoom(gameMode);
                if (roomid == INVALIDKEY) {
                    std::cout << "Fali Create New Room\n";
                    break;
                }

                std::vector<Player*> playerList;
                int matchingPlayerCount = MAXPLAYER;
                for (int i = 0; i < matchingPlayerCount; ++i) {
                    if (readyPlayers.empty()) {
                        break;
                    }
                    playerList.push_back(readyPlayers.top().second);
                    readyPlayers.pop();
                }
                waitingPlayerCount = readyPlayers.size();
                pServer->MatchingComplete(roomid, playerList);
                std::cout << "Start Game room - " << roomid << "| GameMode - " << gameMode << std::endl;
            }

            PushEventGameMatching(pServer->GetTimer());
        }
        catch (const std::exception& e) {
            std::cerr << "[ERROR] : " << e.what() << std::endl;
        }
	}

private:
};