#pragma once
#include "../PacketProcessor.h"
#include "../../../Event/Event.h"

using namespace PacketTable::PlayerTable;

class EventHeartBeat : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		EV_HEART_BEAT* event = reinterpret_cast<EV_HEART_BEAT*>(buf);
		int target = event->sessionID;
		
		if (pServer->GetSessions()[target]->GetIsHeartbeatAck() == false) {
			// 응답이 없었다면 연결 종료
			pServer->Disconnect(target);
		}
		else {
			pServer->GetSessions()[target]->SetIsHeartbeatAck(false);
			pServer->SendHeartBeatPacket(target);
			PushEventHeartBeat(pServer->GetTimer(), target);
		}
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};