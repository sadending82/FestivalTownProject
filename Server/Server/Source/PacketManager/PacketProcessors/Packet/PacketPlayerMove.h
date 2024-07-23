#pragma once
#include "../PacketProcessor.h"

using namespace PacketTable::Player;

class PacketPlayerMove : public PacketProcessor {

public:

	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (verifier.VerifyBuffer<PlayerMove>(nullptr)) {

			const PlayerMove* read = flatbuffers::GetRoot<PlayerMove>(data);

			DEBUGMSGONEPARAM("x -> %f\n", read->pos()->x());
			DEBUGMSGONEPARAM("y -> %f\n", read->pos()->y());
			DEBUGMSGONEPARAM("z -> %f\n", read->pos()->z());
		}
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};