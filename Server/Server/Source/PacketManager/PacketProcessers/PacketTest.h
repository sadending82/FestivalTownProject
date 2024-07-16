#pragma once
#include "PacketProcesser.h"

class PacketTest : public PacketProcesser {

public:

	PacketTest(flatbuffers::FlatBufferBuilder* builder) : PacketProcesser(builder) {}

	void Process(const uint8_t* data, const int size) override {
		flatbuffers::Verifier verifier(data, size);
		if (VerifytestBuffer(verifier)) {

			const test* read_test = Gettest(data);

			DEBUGMSGONEPARAM("Chat -> %s\n", read_test->chat()->c_str());
			DEBUGMSGONEPARAM("Number -> %d\n", read_test->number());
		}

	}

private:
	flatbuffers::FlatBufferBuilder* pBuilder = nullptr;
};