#pragma once
#include "../../PacketManager/PacketManager.h"

#ifdef RunTest

inline constexpr char GameStartCommand = 's';
inline constexpr char SendLifeReduceCommand = 'l';
inline constexpr char DeleteRoom = 'd';
inline constexpr char Gacha = 'g';

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
	bool isRun = true;
	class Server* m_pServer = nullptr;
	PacketManager* m_pPacketManager = nullptr;
	Timer* m_pTimer = nullptr;
};

#endif RunTest