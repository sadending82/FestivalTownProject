#pragma once
#include "../../DB/DB.h"
#include "../flatbuffer/FlatBufferManager.h"
#include "../../Network/Session/Session.h"
#include "../../Server/Server.h"
#include "../../Thread/TimerThread/TimerThread.h"
#include "../../TableManager/TableManager.h"


class Server;
class Timer;
class TableManager;

class PacketProcessor
{
public:
	PacketProcessor() {}
	PacketProcessor(Server* server, PacketSender* packetSender):pServer(server), pPacketSender(packetSender) {}
	~PacketProcessor() {}

	virtual void Process(const uint8_t* data, const int size, const int key) {}
	virtual void Process(unsigned char* buffer) {}

protected:
	Server* pServer = nullptr;
	PacketSender* pPacketSender = nullptr;
};