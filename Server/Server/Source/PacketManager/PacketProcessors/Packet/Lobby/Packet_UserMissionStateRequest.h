#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PassTable;

class Packet_UserMissionStateRequest : public PacketProcessor {

public:
	Packet_UserMissionStateRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (pServer->GetMode() == SERVER_MODE::LIVE && player->GetIsAuthenticated() == false) {
				pServer->Disconnect(key);
				return;
			}

			UserMissionList& playerMissionStateList = player->GetMissionList();

			pServer->GetPacketSender()->SendUserMissionStatePacket(player->GetSessionID(), playerMissionStateList);
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_UserMissionStateRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};