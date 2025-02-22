#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::LoginTable;

class Packet_CheckID_Duplication : public PacketProcessor {

public:
	Packet_CheckID_Duplication(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			const CheckID_Duplication* read = flatbuffers::GetRoot<CheckID_Duplication>(data);

			DB* pDB = pServer->GetDB();

			std::string id = read->id()->c_str();

			if (id.empty()) {
				return;
			}

			int count = pDB->SelectAccountCount(read->id()->c_str());
			flatbuffers::FlatBufferBuilder Builder;

			Builder.Finish(PacketTable::LoginTable::CreateCheckID_Duplication(Builder, Builder.CreateString(id), count));
			std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_CHECK_ID_DUPLICATION, Builder.GetBufferPointer(), Builder.GetSize());
			pServer->GetSessions()[key]->DoSend(send_buffer.data(), send_buffer.size());
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_LoginRequest ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};