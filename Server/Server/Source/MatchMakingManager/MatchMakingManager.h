#pragma once
#include "../protocol.h"
#include "../utility.h"
#include <unordered_map>
#include <set>
#include <mutex>

#define MATCHING_QUEUE		std::set<Player*,  MatchingCompare>
#define MATCHING_QUEUE_MAP	std::unordered_map<eMatchingType, MATCHING_QUEUE>

class MatchMakingManager
{
public:
	MatchMakingManager(class Server* server) :pServer(server) {}

	~MatchMakingManager();

	std::mutex& GetMatchingLock() { return mMatchingLock; }
	MATCHING_QUEUE& GetMatchingQueue(eMatchingType type) { return mMatchingQueue[type]; }

	void CheckMatchMaking(eMatchingType matchingType);

	void MatchingComplete(int roomID, std::vector<class Player*>& players);

private:
	class Server* pServer;

	std::mutex	mMatchingLock;

	MATCHING_QUEUE_MAP mMatchingQueue;
};

