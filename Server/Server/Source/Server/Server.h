#pragma once
#include "../Network/Session/Session.h"
#include "../Room/Room.h"
#include "../PacketManager/PacketSender/PacketSender.h"

#define SESSION_ARRAY std::array<Session*, MAXSESSION>
#define ROOM_ARRAY std::array<Room*, MAXROOM>

constexpr int SERVERPORT = 45872;

class DB;
class Timer;
class TableManager;


class Server
{
public:
	Server();
	~Server();

	int SetSessionID();
	int SetRoomID();
	void Disconnect(int key);
	void Run(class TableManager* pTableManager, class DB* pDB);
	void ThreadJoin();

	void SendAllPlayerInRoomBySessionID(void* packet, int size, int sessionID);
	void SendAllPlayerInRoom(void* packet, int size, int roomID);
	void SendAllPlayerInRoomExceptSender(void* packet, int size, int sessionID);

	std::set<Vector3f> SetObjectSpawnPos(int roomID, int spawnCount);

	void StartHeartBeat(int sessionID);
	int CreateNewRoom(int playerCount, GameMode gameMode);
	void MatchingComplete(int roomID, int playerCnt, std::vector<Player*>& players);
	void StartGame(int roomID);
	void CheckGameEnd(int roomID);
	void TimeoverGameEnd(int roomID);
	int CalculatePoint(GameMode mode, sPlayerGameRecord record);
	int CalculateGoldReward(GameMode mode, int point, bool isMvp);
	void CalculateGameResult(int roomID, uint8_t winningTeams_flag);

	HANDLE GetHcp() { return mHcp; }
	SOCKADDR_IN GetServerAddr() { return mServerAddr; }
	SOCKET GetListenSocket() { return mListenSocket; }
	SESSION_ARRAY& GetSessions() { return mSessions; }
	ROOM_ARRAY& GetRooms() { return mRooms; }
	DB* GetDB() { return mDB; }
	Timer* GetTimer() { return mTimer; }
	TableManager* GetTableManager() { return mTableManager; }
	PacketSender* GetPacketSender() { return mPacketSender; }

private:
	SOCKADDR_IN mServerAddr;
	HANDLE mHcp;
	SOCKET mListenSocket;

	SESSION_ARRAY mSessions = { nullptr };
	ROOM_ARRAY mRooms = { nullptr };
	DB* mDB = nullptr;
	Timer* mTimer = nullptr;
	TableManager* mTableManager = nullptr;
	PacketMaker* mPacketMaker = nullptr;
	PacketSender* mPacketSender = nullptr;
	class TestThread* mTestThreadRef = nullptr;
	std::vector<class WorkerThread*> mWorkerThreadRefs;

	std::vector<std::thread> mWorkerThreads;
	std::thread mTimerThread;
	std::thread mTestThread;

	int testUID = 1;
};

