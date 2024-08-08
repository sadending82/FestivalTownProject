#pragma once
#include "../PacketProcessor.h"
#include "../../../Event/Event.h"

using namespace PacketTable::ObjectTable;

class EventObjectDrop : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		// 방이 활성화 되어있는지 확인해야함
		EV_OBJECT_DROP* event = reinterpret_cast<EV_OBJECT_DROP*>(buf);

		pServer->SendObjectDropPacket(event->roomID);

		PushEventObjectDrop(pServer->GetTimer(), event->roomID);
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};