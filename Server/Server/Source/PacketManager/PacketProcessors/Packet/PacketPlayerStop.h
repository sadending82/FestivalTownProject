#pragma once
#include "../PacketProcessor.h"

using namespace PacketTable::Player;

class PacketPlayerStop : public PacketProcessor {

public:

	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (VerifytestBuffer(verifier)) {

			const PlayerStop* read = flatbuffers::GetRoot<PlayerStop>(data);

			DEBUGMSGONEPARAM("stop x -> %f\n", read->pos()->x());
			DEBUGMSGONEPARAM("stop y -> %f\n", read->pos()->y());
			DEBUGMSGONEPARAM("stop z -> %f\n", read->pos()->z());
		}

	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};