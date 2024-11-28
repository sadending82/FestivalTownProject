#pragma once
#include "../protocol.h"
#include "../utility.h"
#include <unordered_map>
#include <set>
#include <mutex>

//									sessionID - requestTime
#define MATCHING_QUEUE		std::set<std::pair<int, long long>,  MatchingCompare>
#define MATCHING_QUEUE_MAP	std::unordered_map<eMatchingType, MATCHING_QUEUE>

enum eMatchingSequence {
	MS_None = -1,
	MS_1 = 0,
	MS_2 = 1,
	MS_Last = 2
};

struct MatchingCompare {
	bool operator()(const std::pair<int, long long>& a, const std::pair<int, long long>& b) const;
};

struct MapProperties {
	int Map_Index;
	int Map_Theme;
};

class MatchMakingManager
{
public:
	MatchMakingManager(class Server* server);

	~MatchMakingManager();

	std::mutex& GetMatchingLock() { return mMatchingLock; }
	MATCHING_QUEUE& GetMatchingQueue(eMatchingType type) { return mMatchingQueue[type]; }

	int GetMatchingSequence(eMatchingType type) { return mMatchingSequence[type]; }

	void SetMatchingSequence(eMatchingType type, eMatchingSequence sequence) { mMatchingSequence[type] = sequence; }

	bool CheckMatchMaking(eMatchingType matchingType);

	void MatchingComplete(int roomID, std::vector<int>& sessionIDs);

	void UpdateMatchingSequence(eMatchingType matchingType);

	MapProperties SelectRandomMap(GameMode gameMode);

private:
	class Server* pServer;

	// 매치메이킹 작업은 무조건 한 스레드에서만 할 것
	// 멤버 함수 사용, 멤버 변수 읽기/쓰기 작업시 무조건 Lock 사용
	std::mutex	mMatchingLock;


	MATCHING_QUEUE_MAP mMatchingQueue;

	std::unordered_map<eMatchingType, eMatchingSequence> mMatchingSequence;

	std::unordered_map<eMatchingType, std::vector<struct Matching_Table>>* pMatchingDataTables;
};

