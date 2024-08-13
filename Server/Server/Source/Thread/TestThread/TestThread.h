#pragma once
#include "../../PacketManager/PacketManager.h"

#ifdef RunTest

class Timer;

class TestThread
{
public:
	TestThread(class Server* pServer, class Timer* pTimer)
		:m_pServer(pServer),
		m_pTimer(pTimer),
		m_pPacketManager(nullptr)
	{};
	~TestThread() {};

	void RunWorker();

	void GameStart(int roomID);

private:
	class Server* m_pServer;
	PacketManager* m_pPacketManager;
	Timer* m_pTimer;
};

#endif RunTest