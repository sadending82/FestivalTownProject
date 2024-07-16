#pragma once
#include "../Network/Session/Session.h"
#include "../TableManager/TableManager.h"
#include "../PacketManager/PacketManager.h"

constexpr int SERVERPORT = 5000;

class Server
{
public:
	Server() {};
	~Server() {};

	int SetKey();
	void Disconnect(int key);
	void Init(class PacketManager* pPacketManager, class TableManager* pTableManager, class DB* pDB);
	void ThreadRun();

	HANDLE GetHcp() { return mHcp; }
	SOCKADDR_IN GetServerAddr() { return mServerAddr; }
	SOCKET GetListenSocket() { return mListenSocket; }
	std::array<Session*, MAXPLAYER>& GetSessions() { return mSessions; };

private:
	SOCKADDR_IN mServerAddr;
	HANDLE mHcp;
	SOCKET mListenSocket;
	std::array<Session*, MAXPLAYER> mSessions;
	std::vector<std::thread> mWorkerThreads;

	int testUID = 1;
};

