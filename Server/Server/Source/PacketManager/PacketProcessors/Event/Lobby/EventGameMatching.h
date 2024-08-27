#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

class EventGameMatching : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		EV_GAME_MATCHING* event = reinterpret_cast<EV_GAME_MATCHING*>(buf);
        std::cout << "Matching......\n";
        int playerCount = 0;
        for (Session* s : pServer->GetSessions()) {
            s->GetStateLock().lock();
            if (s->GetState() == eSessionState::ST_GAMEREADY) {
                playerCount++;
            }
            s->GetStateLock().unlock();

            // юс╫ц
            if (playerCount >= MINPLAYER) {
                int roomid = pServer->SetRoomID();
                if (roomid == INVALIDKEY) {
                    std::cout << "Fali Create New Room\n";
                }
                else {
                    pServer->MatchingComplete(roomid, playerCount);
                    std::cout << "Start Game room - " << roomid << std::endl;
                }
                break;
            }
        }

        PushEventGameMatching(pServer->GetTimer());
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};