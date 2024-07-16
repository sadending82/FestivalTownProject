#pragma once
#include "../../DB/DB.h"
#include "flatbuffer/FlatBufferManager.h"
#include "../../Network/Session/Session.h"

class PacketProcessor
{
public:
	PacketProcessor() {}
	~PacketProcessor() {}

	virtual void Process(Session* session, const uint8_t* data, const int size) {}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};