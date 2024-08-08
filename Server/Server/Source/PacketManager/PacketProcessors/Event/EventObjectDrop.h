#pragma once
#include "../PacketProcessor.h"
#include "../../../Event/Event.h"

using namespace PacketTable::ObjectTable;

class EventObjectDrop : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		// ���� Ȱ��ȭ �Ǿ��ִ��� Ȯ���ؾ���
		EV_OBJECT_DROP* event = reinterpret_cast<EV_OBJECT_DROP*>(buf);

		pServer->SendObjectDropPacket(event->roomID);

		PushEventObjectDrop(pServer->GetTimer(), event->roomID);
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};