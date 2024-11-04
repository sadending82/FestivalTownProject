#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::LoginTable;

class Packet_VersionCheck : public PacketProcessor {

public:
	Packet_VersionCheck(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<VersionCheck>(nullptr)) {

				const VersionCheck* read = flatbuffers::GetRoot<VersionCheck>(data);

				std::string version = read->version()->c_str();

				
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_LoginRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};