#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"
#include <queue>

class EventGameMatching : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		EV_GAME_MATCHING* event = reinterpret_cast<EV_GAME_MATCHING*>(buf);

        std::priority_queue<std::pair<int, Player*>
            , std::vector<std::pair<int, Player*>>
            , std::greater<std::pair<int, Player*>>> readyPlayers;

        for (Session* s : pServer->GetSessions()) {
            s->GetStateLock().lock();
            if (s->GetState() == eSessionState::ST_MATCHWAITING) {
                readyPlayers.push({s->GetMatchingRequestTime(), dynamic_cast<Player*>(s)});
            }
            s->GetStateLock().unlock();
        }

        int playerCount = readyPlayers.size();

        while(playerCount >= MINPLAYER) {
            int roomid = pServer->CreateNewRoom((playerCount > MAXPLAYER) ? MAXPLAYER : playerCount, GameMode::FITH_Team_Battle_6);
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
                playerCount--;
            }

            pServer->MatchingComplete(roomid, playerCount, playerList);
            std::cout << "Start Game room - " << roomid << std::endl;
        }

        PushEventGameMatching(pServer->GetTimer());
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};