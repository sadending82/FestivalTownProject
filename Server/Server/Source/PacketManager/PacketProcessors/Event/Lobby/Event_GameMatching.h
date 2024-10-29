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

            COUT << "¸ÅÄª ÀÌº¥Æ®: " << event->matchingType << " | " << event->requesterID << " | " << event->requestTime << ENDL;

            MATCHING_QUEUE matchingQueue = MatchMakingManager->GetMatchingQueue(event->matchingType);

            if (matchingQueue.empty()) {
                MatchMakingManager->GetMatchingLock().unlock();
                return;
            }

            int sessionID = matchingQueue.begin()->first;

            if (sessionID != event->requesterID) {
                MatchMakingManager->GetMatchingLock().unlock();
                return;
            }

            Player* player = dynamic_cast<Player*>(pServer->GetSessions()[sessionID]);

            if (player == nullptr) {
                MatchMakingManager->GetMatchingLock().unlock();
                return;
            }

            if (player->GetMatchingRequestTime() != event->requestTime) {
                MatchMakingManager->GetMatchingLock().unlock();
                return;
            }

            bool matchingResult = MatchMakingManager->CheckMatchMaking(event->matchingType);
            MatchMakingManager->UpdateMatchingSequence(event->matchingType);

            MatchMakingManager->GetMatchingLock().unlock();
        }
        catch (const std::exception& e) {
            std::cerr << "[ERROR] : " << e.what() << std::endl;
        }
	}

private:
};