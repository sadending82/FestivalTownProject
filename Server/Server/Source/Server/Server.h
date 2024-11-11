#pragma once
#include "../Network/Session/Session.h"
#include "../Room/Room.h"
#include "../GameManager/FITH/FITH.h"
#include "../TableManager/TableManager.h"
#include "../DB/DB.h"
#include "../PacketManager/PacketSender/PacketSender.h"
#include "../MatchMakingManager/MatchMakingManager.h"
#include "../LobbyManager/LobbyManager.h"

#define SESSION_ARRAY std::array<Session*, MAXSESSION>
#define ROOM_ARRAY std::array<Room*, MAXROOM>
#define GAMEMANAGER_MAP std::unordered_map<GameMode, class GameManager*>

inline constexpr int SERVERPORT = 45872;

inline constexpr int LIVE_MODE = 0;
inline constexpr int TEST_MODE = 1;

inline constexpr std::string_view GAME_VERSION = "0.1";

enum class SERVER_MODE {
	LIVE = 0,
	TEST = 1
};

enum class CONFIG_FIELD {

};

class DB;
class Timer;
class TableManager;


class Server
{
public:
	Server();
	~Server();

	bool						ReadConfig();
	int							SetSessionID();
	int							SetRoomID();
	bool						Disconnect(int key);
	void						Run();
	void						ThreadJoin();
	void						SetGameManagers();

	void						TableReLoad();

	void						MakeTestRoom();

	void						SendAllPlayerInRoomBySessionID(void* packet, int size, int sessionID);
	void						SendAllPlayerInRoom(void* packet, int size, int roomID);
	void						SendAllPlayerInRoomExceptSender(void* packet, int size, int sessionID);

	void						StartHeartBeat(int sessionID);

	std::pair<bool, UserInfo>	UserLogin(const char* accountID, const char* accountPassword, const int sessionID);

	int							CreateNewRoom(GameMode gameMode);

	SERVER_MODE					GetMode() { return mMode; }
	HANDLE						GetHcp() { return mHcp; }
	SOCKADDR_IN					GetServerAddr() { return mServerAddr; }
	SOCKET						GetListenSocket() { return mListenSocket; }
	SESSION_ARRAY&				GetSessions() { return mSessions; }
	ROOM_ARRAY&					GetRooms() { return mRooms; }
	DB*							GetDB() { return mDB; }
	Timer*						GetTimer() { return mTimer; }
	TableManager*				GetTableManager() { return mTableManager; }
	PacketSender*				GetPacketSender() { return mPacketSender; }
	MatchMakingManager*			GetMatchMakingManager() { return mMatchMakingManager; }
	GAMEMANAGER_MAP&			GetGameManagers() { return mGameManagers; }
	LobbyManager*				GetLobbyManager() { return mLobbyManager; }

private:
	SOCKADDR_IN							mServerAddr;
	HANDLE								mHcp;
	SOCKET								mListenSocket;

	SESSION_ARRAY						mSessions = { nullptr };
	ROOM_ARRAY							mRooms = { nullptr };

	DB*									mDB = nullptr;
	Timer*								mTimer = nullptr;
	TableManager*						mTableManager = nullptr;
	class PacketManager*				mPacketManager = nullptr;
	PacketMaker*						mPacketMaker = nullptr;
	PacketSender*						mPacketSender = nullptr;
	MatchMakingManager*					mMatchMakingManager = nullptr;
	LobbyManager*						mLobbyManager = nullptr;

	class TestThread*					mTestThreadRef = nullptr;
	std::vector<class WorkerThread*>	mWorkerThreadRefs;
	GAMEMANAGER_MAP						mGameManagers;

	std::vector<std::thread>			mWorkerThreads;
	std::thread							mTimerThread;
	std::thread							mTestThread;

	std::wstring						mOdbc = L"";
	std::wstring						mDB_ID = L"";
	std::wstring						mDB_Password = L"";

	int									testUID = 1;

	SERVER_MODE							mMode = SERVER_MODE::LIVE;
};

