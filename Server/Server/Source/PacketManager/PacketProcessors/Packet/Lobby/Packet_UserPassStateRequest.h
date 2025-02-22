#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::PassTable;

class Packet_UserPassStateRequest : public PacketProcessor {

public:
	Packet_UserPassStateRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (pServer->GetMode() == SERVER_MODE::LIVE && player->GetIsAuthenticated() == false) {
				pServer->Disconnect(key);
				return;
			}

			const UserPassStateRequest* read = flatbuffers::GetRoot<UserPassStateRequest>(data);

			// юс╫ц
			int pass_index = 601;

			std::unordered_map<int, PlayerPassInfo>& playerPassStateList = player->GetPassInfo();

			pServer->GetPacketSender()->SendUserPassStatePacket(player->GetSessionID(), playerPassStateList[pass_index]);
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_UserPassStateRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};