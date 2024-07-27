#pragma once
#include "../Network/Session/Session.h"
#include "../Room/Room.h"

#define SESSION_ARRAY std::array<Session*, MAXSESSION>
#define ROOM_ARRAY std::array<Room*, MAXROOM>

constexpr int SERVERPORT = 5000;

class Timer;

class Server
{
public:
	Server() {};
	~Server() {};

	int SetSessionKey();
	void Disconnect(int key);
	void Init(class PacketManager* pPacketManager, class TableManager* pTableManager, class DB* pDB);
	void ThreadJoin();

	HANDLE GetHcp() { return mHcp; }
	SOCKADDR_IN GetServerAddr() { return mServerAddr; }
	SOCKET GetListenSocket() { return mListenSocket; }
	SESSION_ARRAY& GetSessions() { return mSessions; }
	ROOM_ARRAY& GetRooms() { return mRooms; }
	Timer* GetTimer() { return mTimer; }

private:
	SOCKADDR_IN mServerAddr;
	HANDLE mHcp;
	SOCKET mListenSocket;

	Timer* mTimer = nullptr;

	SESSION_ARRAY mSessions;
	ROOM_ARRAY mRooms;
	std::vector<std::thread> mWorkerThreads;
	std::thread mTimerThread;

	int testUID = 1;
};

