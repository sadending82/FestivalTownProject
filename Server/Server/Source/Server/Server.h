#pragma once
#include "../Network/Session/Session.h"
#include "../Room/Room.h"
#include "../PacketManager/PacketMaker/PacketMaker.h"

#define SESSION_ARRAY std::array<Session*, MAXSESSION>
#define ROOM_ARRAY std::array<Room*, MAXROOM>

constexpr int SERVERPORT = 45872;

class Timer;

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

	void SendPlayerAdd(int sessionID, int destination);
	void SendGameMatchingResponse(int sessionID);
	void SendGameStart(int roomID);
	void SendAllPlayerReady(int roomID);

	void SendHeartBeatPacket(int sessionID);
	void SendBlockDropPacket(int roomID, int spawnCount);
	void SendBombSpawnPacket(int roomID, int spawnCount);
	void SendBombExplosionPacket(int roomID, int bombID);
	void SendLifeReducePacket(int team, int lifeCount, int roomID);
	void SendRemainTimeSync(int roomID);
	void SendGameStartPacket(int roomID);
	void SendGameEndPacket(int roomID, uint8_t winningTeams_flag);
	void SendGameHostChange(int sessionID);
	void SendPlayerDeadPacket(int inGameID, int roomID);
	void SendPlayerRespawn(int inGameID, int roomID);
	void SendWeaponSpawnPacket(int roomID, int spawnCount);
	void SendPlayerCalculatedDamage(int targetID, int roomID, int attackType, int hp, int damageAmount, Vector3f knockback_direction);

	std::set<Vector3f> SetObjectSpawnPos(int roomID, int spawnCount);

	void StartHeartBeat(int sessionID);
	int CreateNewRoom(int playerCount, GameCode gameMode);
	void MatchingComplete(int roomID, int playerCnt, std::vector<Player*>& players);
	void StartGame(int roomID);
	void CheckGameEnd(int roomID);
	void TimeoverGameEnd(int roomID);

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

	Timer* mTimer = nullptr;
	TableManager* mTableManager = nullptr;
	PacketMaker* mPacketMaker = nullptr;

	SESSION_ARRAY mSessions;
	ROOM_ARRAY mRooms;
	std::vector<std::thread> mWorkerThreads;
	std::thread mTimerThread;
	std::thread mTestThread;

	class TestThread* mTestThreadRef;
	std::vector<class WorkerThread*> mWorkerThreadRefs;

	int testUID = 1;
};

