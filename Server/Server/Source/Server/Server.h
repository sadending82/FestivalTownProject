#pragma once
#include "../Network/Session/Session.h"
#include "../PacketManager/PacketManager.h"
#include "../TableManager/TableManager.h"

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
	array<Session*, MAXPLAYER>& GetSessions() { return mSessions; };

private:
	SOCKADDR_IN mServerAddr;
	HANDLE mHcp;
	SOCKET mListenSocket;
	array<Session*, MAXPLAYER> mSessions;
	vector<thread> mWorkerThreads;

	int testUID = 1;
};

