#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"
#include <queue>

class EventGameMatching : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		EV_GAME_MATCHING* event = reinterpret_cast<EV_GAME_MATCHING*>(buf);
        std::cout << "Matching......\n";

        std::queue<Player*> readyPlayers;

        for (Session* s : pServer->GetSessions()) {
            s->GetStateLock().lock();
            if (s->GetState() == eSessionState::ST_GAMEREADY) {
                readyPlayers.push(dynamic_cast<Player*>(s));
            }
            s->GetStateLock().unlock();
        }

        int playerCount = readyPlayers.size();

        while(playerCount >= MINPLAYER) {
            int roomid = pServer->CreateNewRoom((playerCount > MAXPLAYER) ? MAXPLAYER : playerCount, GameCode::FITH_Team_battle_Three);
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
                playerList.push_back(readyPlayers.front());
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