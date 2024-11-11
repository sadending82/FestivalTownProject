#pragma once
#include "../protocol.h"
#include "../utility.h"
#include <unordered_map>

class LobbyManager
{
public:

	LobbyManager(class Server* server);
	~LobbyManager();

	struct GachaItem RollGacha(std::unordered_map<int, struct GachaItem>& gachaItemList);
	bool GiveGachaItemToUser(int uid, int payItem, int price, struct GachaItem& gachaItem);

private:

	class Server* pServer = nullptr;
	class TableManager* pTableManager = nullptr;
	class DB* pDB = nullptr;
};

