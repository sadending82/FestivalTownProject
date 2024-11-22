#pragma once
#include "../protocol.h"
#include "../utility.h"
#include "../DB/Schema/Schemas.h"
#include <unordered_map>

class LobbyManager
{
public:

	LobbyManager(class Server* server);
	~LobbyManager();

	void CheckAttendanceEvent(int uid, std::unordered_map<int, std::vector<sDayAttendanceInfo>>& attendanceInfoList);

	void CheckReadyToGamePlay(class Room* room, int roomID);

	struct GachaItem RollGacha(std::unordered_map<int, struct GachaItem>& gachaItemList);
	bool GiveGachaItemToUser(int uid, int payItem, int price, struct GachaItem& gachaItem);

private:

	class Server* pServer = nullptr;
	class TableManager* pTableManager = nullptr;
	class DB* pDB = nullptr;
};

