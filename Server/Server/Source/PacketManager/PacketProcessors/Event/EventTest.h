#pragma once
#include "../PacketProcessor.h"

class EventTest : public PacketProcessor {

public:

	virtual void Process(Server* pServer, const uint8_t* data, const int size) {
		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (VerifytestBuffer(verifier)) {

			const test* read_test = Gettest(data);

			DEBUGMSGONEPARAM("Chat -> %s\n", read_test->chat()->c_str());
			DEBUGMSGONEPARAM("Number -> %d\n", read_test->number());
		}

		auto chat = mBuilder.CreateString("Event Working");
		int number = 11;

		mBuilder.Finish(Createtest(mBuilder, chat, number));

		std::vector<uint8_t> buffer = MakeEventBuffer(0, mBuilder.GetBufferPointer(), mBuilder.GetSize(), 10000);
		pServer->GetTimer()->PushEvent(buffer);
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};