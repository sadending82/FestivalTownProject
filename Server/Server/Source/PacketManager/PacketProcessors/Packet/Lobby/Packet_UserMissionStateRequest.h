#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PassTable;

class Packet_UserMissionStateRequest : public PacketProcessor {

public:
	Packet_UserMissionStateRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<UserMissionStateRequest>(nullptr)) {

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				if (pServer->GetMode() == SERVER_MODE::LIVE) {
					if (player->GetIsAuthenticated() == false) {
						pServer->Disconnect(key);
						return;
					}
				}

				//юс╫ц
				int pass_index = 601;

				std::unordered_map<int, PlayerPassInfo>& playerPassStateList = player->GetPassInfo();

				pServer->GetPacketSender()->SendUserPassStatePacket(player->GetSessionID(), playerPassStateList[pass_index]);

			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_UserMissionStateRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};