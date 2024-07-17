#pragma once
#include "PacketProcessor.h"

class Server;

class PacketTest : public PacketProcessor {

public:

	virtual void Process(std::array<Session*, MAXPLAYER>& sessions, const uint8_t* data, const int size, const int key) {

		mBuilder.Clear();

		flatbuffers::Verifier verifier(data, size);
		if (VerifytestBuffer(verifier)) {

			const test* read_test = Gettest(data);

			DEBUGMSGONEPARAM("Chat -> %s\n", read_test->chat()->c_str());
			DEBUGMSGONEPARAM("Number -> %d\n", read_test->number());
		}

		auto chat = mBuilder.CreateString("hi client");
		int number = 11;

		mBuilder.Finish(Createtest(mBuilder, chat, number));

		std::vector<uint8_t> send_buffer = MakeBuffer(1, mBuilder.GetBufferPointer(), mBuilder.GetSize());

		sessions[key]->DoSend(&send_buffer);

	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};