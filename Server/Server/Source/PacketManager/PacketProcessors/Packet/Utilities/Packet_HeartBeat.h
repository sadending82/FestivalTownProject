#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::UtilitiesTable;

class Packet_HeartBeat : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		// ������ ���� ������ ��� X ��ȿ�� ���������� Ȯ���ؼ� ó��
		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<HeartBeat>(nullptr)) {
			const HeartBeat* read = flatbuffers::GetRoot<HeartBeat>(data);

			pServer->GetSessions()[key]->SetIsHeartbeatAck(true);
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};