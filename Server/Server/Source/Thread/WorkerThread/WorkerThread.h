#pragma once
#include "../../PacketManager/PacketManager.h"

class WorkerThread
{
public:
	WorkerThread(class Server* pServer)
		:m_pServer(pServer),
		m_pPacketManager(nullptr)
	{};
	~WorkerThread();

	void RunWorker();

private:
	class Server* m_pServer;
	PacketManager* m_pPacketManager;
};

