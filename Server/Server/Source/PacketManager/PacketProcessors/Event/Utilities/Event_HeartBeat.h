#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

using namespace PacketTable::PlayerTable;

class Event_HeartBeat : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		try {
			EV_HEART_BEAT* event = reinterpret_cast<EV_HEART_BEAT*>(buf);
			int target = event->sessionID;

			if (pServer->GetSessions()[target]->GetIsHeartbeatAck() == false) {
				// ������ �����ٸ� ���� ����
				pServer->Disconnect(target);
			}
			else {
				pServer->StartHeartBeat(target);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[ERROR] : " << e.what()  << std::endl;
		}
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};