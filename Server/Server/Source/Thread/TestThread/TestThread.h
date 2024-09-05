#pragma once
#include "../../PacketManager/PacketManager.h"

#ifdef RunTest

constexpr char GameStartCommand = 's';
constexpr char SendLifeReduceCommand = 'l';
constexpr char DeleteRoom = 'd';

class Timer;

class TestThread
{
public:
	TestThread(class Server* pServer, class Timer* pTimer)
		:m_pServer(pServer),
		m_pTimer(pTimer),
		m_pPacketManager(nullptr)
	{};
	~TestThread();

	void RunWorker();

	int GetReadyPlayerCnt();

private:
	class Server* m_pServer;
	PacketManager* m_pPacketManager;
	Timer* m_pTimer;
};

#endif RunTest