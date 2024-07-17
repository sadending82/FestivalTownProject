#pragma once
#include "../../DB/DB.h"
#include "flatbuffer/FlatBufferManager.h"
#include "../../Network/Session/Session.h"

class PacketProcessor
{
public:
	PacketProcessor() {}
	~PacketProcessor() {}

	virtual void Process(std::array<Session*, MAXPLAYER>& sessions, const uint8_t* data, const int size, const int key) {}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};