#pragma once
#include "../protocol.h"
#include "../utility.h"
#include "../DB/Schema/Schemas.h"
#include "../TableManager/Tables/Tables.h"
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
	bool GiveGachaItemToUser(Player* player, int payItem, int price, struct GachaItem& gachaItem);

	void LoadPassState(class Player* player);

	void LoadMissionProgress(class Player* player);

	bool UpdateGachaMission(class Player* player, int itemCode);

	bool UpdateLoginMission(class Player* player);

	bool CheckCompleteMission(class Player* player, int missionCode);

	bool GiveMissionReward(class Player* player, PassMissionInfo& missionInfo);

	bool CheckPassLevelUp(PlayerPassInfo& playerPassInfo, int passIndex);

	bool GivePassReward(class Player* player, int pass_index, int pass_type, int level);

	bool PurchaseShopGoods(class Player* player, Shop_Goods& goodsInfo, int goods_index);

	bool PurchasePass(class Player* player, Shop_Goods& goodsInfo, int goods_index);
	
private:

	class Server* pServer = nullptr;
	class TableManager* pTableManager = nullptr;
	class DB* pDB = nullptr;

	std::unordered_map<int, int> shopGoodsList;
};