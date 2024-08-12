#pragma once
#include "../Network/Session/Session.h"
#include "../Room/Room.h"
#include "../PacketManager/PacketProcessors/flatbuffer/FlatBufferManager.h"

#define SESSION_ARRAY std::array<Session*, MAXSESSION>
#define ROOM_ARRAY std::array<Room*, MAXROOM>

constexpr int SERVERPORT = 45872;

class Timer;

class Server
{
public:
	Server() {};
	~Server() {};

	int SetSessionKey();
	void Disconnect(int key);
	void Init(class TableManager* pTableManager, class DB* pDB);
	void ThreadJoin();

	void SendAllPlayerInRoomBySessionID(void* packet, int size, int sessionID);
	void SendAllPlayerInRoom(void* packet, int size, int roomID);
	void SendAllPlayerInRoomExceptSender(void* packet, int size, int sessionID);

	void SendPlayerAdd(int sessionID, int destination);
	void SendPlayerGameInfo(int sessionID);
	void SendHeartBeatPacket(int sessionID);
	void SendObjectDropPacket(int roomID, int spawnCount);
	void SendBombSpawnPacket(int roomID, int spawnCount);

	void StartHeartBeat(int sessionID);

	HANDLE GetHcp() { return mHcp; }
	SOCKADDR_IN GetServerAddr() { return mServerAddr; }
	SOCKET GetListenSocket() { return mListenSocket; }
	SESSION_ARRAY& GetSessions() { return mSessions; }
	ROOM_ARRAY& GetRooms() { return mRooms; }
	Timer* GetTimer() { return mTimer; }
	TableManager* GetTableManager() { return mTableManager; }

private:
	SOCKADDR_IN mServerAddr;
	HANDLE mHcp;
	SOCKET mListenSocket;

	flatbuffers::FlatBufferBuilder mBuilder;

	Timer* mTimer = nullptr;
	TableManager* mTableManager = nullptr;

	SESSION_ARRAY mSessions;
	ROOM_ARRAY mRooms;
	std::vector<std::thread> mWorkerThreads;
	std::thread mTimerThread;

	int testUID = 1;
};

