#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::GameTable;

class PacketGameReady : public PacketProcessor {

public:
	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<GameReady>(nullptr)) {

			const GameReady* read = flatbuffers::GetRoot<GameReady>(data);


		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};