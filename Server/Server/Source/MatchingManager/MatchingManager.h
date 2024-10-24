#pragma once
#include "../utility.h"
#include <unordered_map>
#include <set>
#include <mutex>

#define MATCHING_QUEUE		std::set<Player*,  MatchingCompare>
#define MATCHING_QUEUE_MAP	std::unordered_map<eMatchingType, MATCHING_QUEUE>

enum eMatchingType {
	FITH_SOLO = 110,
	FITH_TEAM = 120
};

class MatchingManager
{
public:
	MatchingManager(class Server* server) :pServer(server) {}

	~MatchingManager();

	std::mutex& GetMatchingLock() { return mMatchingLock; }
	MATCHING_QUEUE& GetMatchingQueue(eMatchingType type) { return mMatchingQueue[type]; }

	void MatchingComplete(int roomID, std::vector<class Player*>& players);

private:
	class Server* pServer;

	std::mutex	mMatchingLock;

	MATCHING_QUEUE_MAP mMatchingQueue;
};

