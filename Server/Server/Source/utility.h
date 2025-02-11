#pragma once
#include "protocol.h"
#include "DB/Schema/Schemas.h"

#include <iostream>
#include <chrono>
#include <vector>
#include <unordered_map>

#define TIMEPOINT std::chrono::system_clock::time_point

inline constexpr int SERVERKEY = 0;
inline constexpr int STARTKEY = 1;
inline constexpr int STARTBOTKEY = 4001;
inline constexpr int INVALIDKEY = -1;
inline constexpr int TESTROOM = 0;

inline constexpr int BUFSIZE = 4000;

inline constexpr int MAXSESSION = 5000;
inline constexpr int MAXUSER = 4000;
inline constexpr int MAXROOM = 2000;
inline constexpr int MAXPLAYER = 6;
inline constexpr int MINPLAYER = 2;

inline constexpr int BLOCKSIZE = 2; // meter

inline constexpr float offsetX = 20;
inline constexpr float offsetZ = 10;

inline constexpr int NONE_ITEM_INDEX = 0;

#ifdef _DEBUG
#define DEBUGMSGNOPARAM(args) (printf("file: %s, line:%d ",__FILE__,__LINE__), printf(args))
#define DEBUGMSGONEPARAM(args,k) (printf("file: %s, line:%d ",__FILE__,__LINE__), printf(args,k))
#else
#define DEBUGMSGNOPARAM(...)
#define DEBUGMSGONEPARAM(...)
#endif

#define RunTest

enum class eSessionState
{
	ST_FREE,
	ST_ACCEPTED,
	ST_MATCHWAITING,
	ST_GAMELOADING,
	ST_INGAME
};

enum class ePlayerState {
	PS_NONE,
	PS_ALIVE,
	PS_DEAD,
	PS_GROGGY
};

enum class eRoomState {
	RS_FREE,
	RS_INGAME
};

enum class eObjectType {
	WEAPON,
	BOMB
};

#pragma pack (push, 1)

struct HEADER {
public:
	uint16_t	flatBufferSize;
	uint16_t	type;
};

struct Vector3f {
	float x;
	float y;
	float z;

	Vector3f() : x(0), y(0), z(0) {};
	Vector3f(float fx, float fy, float fz) : x(fx), y(fy), z(fz) {};

	bool operator == (const Vector3f& other) const {
		if (x == other.x && y == other.y && z == other.z) {
			return true;
		}
		return false;
	}

	bool operator < (const Vector3f& other) const {
		if (x != other.x) return x < other.x;
		if (y != other.y) return y < other.y;
		return z < other.z;
	}
};

struct sGameReward {
	int index = 0;
	int value = 0;
};

struct sPlayerGameRecord {
	int team;
	int is_mvp;
	BattleResult battleResult;

	std::vector<sGameReward> rewards;

	UserGameRecords gameRecord;

	sPlayerGameRecord() : team(0), is_mvp(false), battleResult(BattleResult::BR_Draw){};

	void Init() {
		team = 0;
		rewards.clear();
		is_mvp = false;
		battleResult = BattleResult::BR_Draw;
		gameRecord.Init();
	}
};

struct sMatchingInfo {
	int RoomID = 0;
	int Team = 0;
	int GameMode = 0;
	int MapIndex = 0;
	int MapTheme = 0;
	int GameTime = 0;
	int TotalPlayerCount = 0;
	bool IsHost = false;

	sMatchingInfo() {};
	sMatchingInfo(int roomid, int team, int gameMode, int mapIndex, int mapTheme, int gameTime, int totalPlayerCount, int isHost)
		: RoomID(roomid), Team(team), GameMode(gameMode), MapIndex(mapIndex), MapTheme(mapTheme), GameTime(gameTime), TotalPlayerCount(totalPlayerCount), IsHost(isHost) {};
};

struct Trie {
	bool finish;
	std::unordered_map<wchar_t, Trie*> next;

	Trie() : finish(false) {
	}

	~Trie() {
		for (auto& n : next) {
			delete n.second;
		}
	}

	void Insert(const wchar_t* string) {
		if (*string == L'\0') {
			finish = true;
		}
		else {
			wchar_t curr = *string;
			if (next.find(curr) == next.end()) {
				next[curr] = new Trie();
			}
			next[curr]->Insert(string + 1);
		}
	}

	Trie* Find(const wchar_t* string) {
		if (*string == L'\0') {
			return this;
		}
		wchar_t curr = *string;
		if (next.find(curr) == next.end()) {
			return NULL;
		}
		return next[curr]->Find(string + 1);
	}

	bool Search(const std::wstring& string) {
		for (int i = 0; i < string.size(); ++i) {
			Trie* node = this;

			for (int j = i; j < string.size(); ++j) {
				wchar_t c = string[j];

				if (node->next.find(c) == node->next.end()) {
					break;
				}

				node = node->next[c];

				if (node->finish == true) {
					return true;
				}
			}
		}

		return false;
	}
};

struct PlayerPassInfo {
	UserPass passState;

	// <level, <type, isRewarded>>
	std::unordered_map<int, std::unordered_map<int, bool>> isRewardedList;

	void Init(UserPass& userPass) { passState = userPass; }

	void SetExp(int value) { passState.passExp == value; }

	void SetLevel(int value) { passState.passLevel == value; }

	void SetIsRewarded(int level, int type) { isRewardedList[level][type] = true; }
};

#pragma pack(pop)

std::vector<uint8_t> MakeBuffer(const int type, const uint8_t* data, const int size);

Vector3f ConvertVec3fToVec2i(Vector3f vec);
Vector3f ConvertVec2iToVec3f(int x, int z);

std::string wstringToString(const std::wstring& input);

sCharacterCustomizing DeserializationCharacterCustomizing(const std::vector<uint8_t>& buffer);
std::vector<uint8_t> SerializationCharacterCustomizing(sCharacterCustomizing characterCustomizing);

bool CheckDateToday(const std::tm& date);