#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

using namespace PacketTable::PlayerTable;

class Event_HeartBeat : public PacketProcessor {

public:
	Event_HeartBeat(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(unsigned char* buf) {
		try {
			EV_HEART_BEAT* event = reinterpret_cast<EV_HEART_BEAT*>(buf);
			int target = event->sessionID;

			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[target]);

			if (player == nullptr) {
				return;
			}

			if (player->GetIsHeartbeatAck() == false) {
				// 응답이 없었다면 연결 종료
				bool result = pServer->Disconnect(target);

				if (result == true && player->GetUID() != INVALIDKEY) {
					pServer->GetDB()->UpdateUserConnectionState(player->GetUID(), false);
				}
			}
			else {
				pServer->StartHeartBeat(target);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Event_HeartBeat ERROR] : " << e.what()  << std::endl;
		}
	}

private:
};