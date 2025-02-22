#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::LoginTable;

class Packet_VersionCheckRequest : public PacketProcessor {

public:
	Packet_VersionCheckRequest(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			const VersionCheckRequest* read = flatbuffers::GetRoot<VersionCheckRequest>(data);

			std::string version = read->version()->c_str();

			ERROR_CODE result = ERROR_CODE::ER_NONE;

			if (version != GAME_VERSION) {
				pPacketSender->SendVersionCheckResponsePacket(key, ERROR_CODE::ER_VERSION_MISMATCH);
				pServer->Disconnect(key);
				return;
			}

			pPacketSender->SendVersionCheckResponsePacket(key, ERROR_CODE::ER_NONE);
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_LoginRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};