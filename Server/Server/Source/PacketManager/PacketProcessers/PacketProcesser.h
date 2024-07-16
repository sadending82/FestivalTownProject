#pragma once
#include "../../DB/DB.h"
#include "flatbuffer/FlatBufferManager.h"

class PacketProcesser
{
public:
	
	PacketProcesser() {}

	PacketProcesser(flatbuffers::FlatBufferBuilder* builder) : pBuilder(builder) {}

	virtual void Process(const uint8_t* data, const int size) {}
private:

	flatbuffers::FlatBufferBuilder* pBuilder = nullptr;
};