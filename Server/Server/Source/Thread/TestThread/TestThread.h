#pragma once
#include "../../PacketManager/PacketManager.h"

#ifdef RunTest

constexpr char GameStartCommand = 's';
constexpr char SendLifeReduceCommand = 'l';
constexpr char DeleteRoom = 'd';
constexpr char Gacha = 'g';
constexpr char TableReload = 't';

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


private:
	bool isRun = true;
	class Server* m_pServer = nullptr;
	PacketManager* m_pPacketManager = nullptr;
	Timer* m_pTimer = nullptr;
};

#endif RunTest