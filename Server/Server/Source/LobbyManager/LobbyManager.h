#pragma once
#include "../protocol.h"
#include "../utility.h"
#include "../DB/Schema/Schemas.h"
#include <unordered_map>
#include <set>

class LobbyManager
{
public:

	LobbyManager(class Server* server);
	~LobbyManager();

	void CheckAndLoadUserAttendanceEvent(class Player* player, std::unordered_map<int, std::set<sDayAttendanceInfo>>& attendanceInfoList);

	void CheckReadyToGamePlay(class Room* room, int roomID);

	bool CheckIsNewEvent(std::tm beforeLoginTime);

	struct GachaItem RollGacha(std::unordered_map<int, struct GachaItem>& gachaItemList);
	bool GiveGachaItemToUser(int uid, int payItem, int price, struct GachaItem& gachaItem);

	void LoadPassState(class Player* player);

	void LoadMissionProgress(class Player* player);

	bool UpdateGachaMission(class Player* player, int itemCode);

	bool UpdateLoginMission(class Player* player);

	bool CheckCompleteMission(class Player* player, int missionCode);

	bool GiveMissionReward(class Player* player, PassMissionInfo& missionInfo);

	bool CheckPassLevelUp(class Player* player, PlayerPassInfo& playerPassInfo);

	bool GivePassReward(class Player* player, int pass_index, int pass_type, int reward_level);
	
private:

	class Server* pServer = nullptr;
	class TableManager* pTableManager = nullptr;
	class DB* pDB = nullptr;
};