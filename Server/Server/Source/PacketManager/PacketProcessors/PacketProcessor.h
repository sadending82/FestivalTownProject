#pragma once
#include "../../DB/DB.h"
#include "flatbuffer/FlatBufferManager.h"
#include "../../Network/Session/Session.h"
#include "../../Server/Server.h"
#include "../../Thread/TimerThread/TimerThread.h"

class Server;
class Timer;

class PacketProcessor
{
public:
	PacketProcessor() {}
	~PacketProcessor() {}

	virtual void Process(Server* pServer, const uint8_t* data, const int size, const int key) {}
	virtual void Process(Server* pServer, unsigned char* buffer) {}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};