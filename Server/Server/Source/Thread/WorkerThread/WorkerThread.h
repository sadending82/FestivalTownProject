#pragma once
#include "../../PacketManager/PacketManager.h"

class WorkerThread
{
public:
	WorkerThread(class Server* pServer)
		:pServer(pServer),
		pPacketManager(nullptr)
	{};
	~WorkerThread();

	void RunWorker();

private:
	bool isRun = true;

	class Server* pServer;
	PacketManager* pPacketManager;
};

