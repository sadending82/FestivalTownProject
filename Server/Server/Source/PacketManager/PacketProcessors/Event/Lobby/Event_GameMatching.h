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

            MatchMakingManager->CheckMatchMaking(eMatchingType::FITH_TEAM);

            PushEventGameMatching(pServer->GetTimer());
        }
        catch (const std::exception& e) {
            std::cerr << "[ERROR] : " << e.what() << std::endl;
        }
	}

private:
};