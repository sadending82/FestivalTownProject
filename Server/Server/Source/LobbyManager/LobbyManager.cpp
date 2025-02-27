#pragma once
#include "LobbyManager.h"
#include "../Server/Server.h"
#include <random>

LobbyManager::LobbyManager(Server* server)
{
	pServer = server;
	pTableManager = server->GetTableManager();
	pDB = server->GetDB();


}

LobbyManager::~LobbyManager()
{

}

void LobbyManager::CheckAndLoadUserAttendanceEvent(Player* player, std::unordered_map<int, std::set<sDayAttendanceInfo>>& attendanceInfoList)
{
	int uid = player->GetUID();

	std::time_t nowTime = std::time(nullptr);
	std::tm tNowTime = {};

	localtime_s(&tNowTime, &nowTime);

	for (auto& eventInfo : pTableManager->GetEventList()) {

		int eventCode = eventInfo.first;

		std::time_t openTime = std::mktime(const_cast<std::tm*>(&eventInfo.second.Open_Date));
		std::time_t closeTime = std::mktime(const_cast<std::tm*>(&eventInfo.second.Close_Date));

		if (nowTime < openTime || closeTime < nowTime) {
			continue;
		}

		if (eventInfo.second.Type == 2) {
			attendanceInfoList[eventCode] = pDB->SelectUserAttendanceEvent(uid, eventCode);

			int nextDayCount = 0;
			sDayAttendanceInfo latelyAttendance;
			if (attendanceInfoList[eventCode].empty() == false) {
				latelyAttendance = *std::prev(attendanceInfoList[eventCode].end());
				nextDayCount = latelyAttendance.day_number + 1;
			}
			else {
				nextDayCount = 1;
			}

			if (pTableManager->GetEventRewardList()[eventCode].count(nextDayCount) != 0) {

				// 오늘 출석 안했으면 출석 처리
				if (tNowTime.tm_year != latelyAttendance.attendance_date.tm_year
					|| tNowTime.tm_mon != latelyAttendance.attendance_date.tm_mon
					|| tNowTime.tm_mday != latelyAttendance.attendance_date.tm_mday) {
					sDayAttendanceInfo nextAttendance;
					localtime_s(&nextAttendance.attendance_date, &nowTime);
					nextAttendance.day_number = nextDayCount;
					nextAttendance.is_rewarded = false;

					if (pDB->InsertUserAttendance(uid, eventCode, nextDayCount) == ERROR_CODE::ER_NONE) {
						attendanceInfoList[eventCode].insert(nextAttendance);
					}
				}
			}

		}
	}
}

void LobbyManager::CheckReadyToGamePlay(Room* room, int roomID)
{
	if (room->CheckAllPlayerReady() == true) {
		if (room->SetAllPlayerReady(true) == true) {
			room->ChangeAllPlayerInGame();
			pServer->GetPacketSender()->SendAllPlayerReady(roomID);
			pServer->GetGameManagers()[room->GetGameMode()]->PrepareForStartGame(room, roomID);
		}
	}
}

bool LobbyManager::CheckIsNewEvent(std::tm beforeLoginTime)
{
	std::time_t NowTime = std::time(nullptr);
	std::time_t lastLogintime = std::mktime(const_cast<std::tm*>(&beforeLoginTime));

	for (auto& eventInfo : pTableManager->GetEventList()) {

		int eventCode = eventInfo.first;

		std::time_t openTime = std::mktime(const_cast<std::tm*>(&eventInfo.second.Open_Date));
		std::time_t closeTime = std::mktime(const_cast<std::tm*>(&eventInfo.second.Close_Date));

		if (NowTime < openTime || closeTime < NowTime) {
			continue;
		}
		if (lastLogintime < openTime) {
			return true;
		}
	}
	return false;
}

GachaItem LobbyManager::RollGacha(std::unordered_map<int, GachaItem>& gachaItemList)
{
	int totalWeight = 0;

	for (auto& item : gachaItemList) {
		totalWeight += item.second.Gacha_Weight;
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, totalWeight);

	int randomWeight = dis(gen);
	int cumulativeWeight = 0;

	for (auto& item : gachaItemList) {
		cumulativeWeight += item.second.Gacha_Weight;
		if (randomWeight <= cumulativeWeight) {
			 
			return item.second;
		}

	}
}

bool LobbyManager::GiveGachaItemToUser(Player* player, int payItem, int price, GachaItem& gachaItem)
{
	const int uid = player->GetUID();
	int itemIndex = gachaItem.Reward_Item_Index;
	const int itemAmount = gachaItem.Reward_Item_Value;
	const int itemType = (int)pTableManager->GetItemInfos()[itemIndex].Item_Type;
	const int mileageIndex = 100003;
	std::unordered_map<int, UserItem>& playerItems = player->GetItems();

	const ERROR_CODE payResult = pDB->UpdateUserItemCount(uid, payItem, -price);
	if (payResult == ERROR_CODE::ER_DB_ERROR || payResult == ERROR_CODE::ER_DB_NO_DATA) {
		return false;
	}
	else {
		playerItems[payItem].count -= price;
		pDB->UpsertUserCurrencyRecord(uid, payItem, 0, price);
	}

	switch (itemType) {
	case(int)ItemType::Money:
	{
		const ERROR_CODE result = pDB->UpsertUserItemCount(uid, itemIndex, itemAmount);
		if (result == ERROR_CODE::ER_DB_ERROR) {
			// 실패 시 재화 다시 돌려주고 실패 반환
			pDB->UpdateUserItemCount(uid, payItem, price);
			playerItems[payItem].count += price;
			return false;
		}
		else {
			playerItems[itemIndex].owner_UID = uid;
			playerItems[itemIndex].itemCode = itemIndex;
			playerItems[itemIndex].itemType = itemType;
			playerItems[itemIndex].count += itemAmount;

			pDB->UpsertUserCurrencyRecord(uid, itemIndex, itemAmount, 0);
		}
	}
	break;
	case (int)ItemType::Skin:
	case (int)ItemType::Accessory_Back:
	case (int)ItemType::Accessory_Face:
	case (int)ItemType::Accessory_Head:
	case (int)ItemType::Emotion:
	{
		int currItemCount = playerItems.count(itemIndex);
		// 새로 얻은 경우
		if (currItemCount == 0) {
			const ERROR_CODE result = pDB->InsertUserItem(uid, itemIndex, itemAmount, itemType);
			if (result == ERROR_CODE::ER_DB_ERROR) {
				// 실패 시 재화 다시 돌려주고 실패 반환
				pDB->UpdateUserItemCount(uid, payItem, price);
				playerItems[payItem].count += price;
				return false;
			}
			else {
				playerItems[itemIndex].owner_UID = uid;
				playerItems[itemIndex].itemCode = itemIndex;
				playerItems[itemIndex].itemType = itemType;
				playerItems[itemIndex].count = itemAmount;
			}
		}
		// 이미 있는 경우
		else {
			// 마일리지 지급
			const int itemGrade = (int)pTableManager->GetItemInfos()[itemIndex].Item_Grade;
			const int mileage = pTableManager->GetGachaAcquiredMileages()[itemGrade];
			ERROR_CODE result = pDB->UpsertUserItemCount(uid, mileageIndex, mileage);
			if (result == ERROR_CODE::ER_DB_ERROR) {
				// 실패 시 재화 다시 돌려주고 실패 반환
				pDB->UpdateUserItemCount(uid, payItem, price);
				return false;
			}
			else {
				playerItems[mileageIndex].itemCode = mileageIndex;
				playerItems[mileageIndex].count += mileage;

				pDB->UpsertUserCurrencyRecord(uid, mileageIndex, mileage, 0);
			}


			gachaItem.Reward_Item_Index = mileageIndex;
			gachaItem.Reward_Item_Value = mileage;
		}
	}
	break;
	}

	return true;
}

void LobbyManager::LoadPassState(Player* player)
{
	int uid = player->GetUID();

	std::unordered_map<int, PlayerPassInfo>& playerPassStateList = player->GetPassInfo();

	std::unordered_map<int, PassInfo>& passList = pTableManager->GetPassList();

	time_t now = std::time(nullptr);
	std::tm today{};
	localtime_s(&today, &now);

	std::tm lastLoginDate = player->GetLastLoginTime();


	for (auto& passInfo : passList) {

		int passIndex = passInfo.second.index;

		std::time_t openTime = std::mktime(const_cast<std::tm*>(&passInfo.second.open_date));
		std::time_t closeTime = std::mktime(const_cast<std::tm*>(&passInfo.second.close_date));

		if (now < openTime || closeTime < now) {
			continue;
		}

		UserPass passState = pDB->SelectUserPass(uid, passInfo.second.index);

		// db에 패스 데이터가 없으면 초기값으로 초기화
		if (passState.passIndex == 0) {
			passState.passIndex = passIndex;
			passState.passType = ePassType::PT_NORMAL;
			passState.passLevel = 0;
			passState.passExp = 0;
			passState.daily_mission_exp = 0;
		}
		else {
			if (today.tm_year != lastLoginDate.tm_year
				|| today.tm_mon != lastLoginDate.tm_mon
				|| today.tm_mday != lastLoginDate.tm_mday) {

				passState.daily_mission_exp = 0;

				pDB->UpsertUserPass(uid, passState);
			}
		}

		playerPassStateList[passIndex].Init(passState);

		std::vector<UserPassReward> passRewardStates = pDB->SelectUserPassReward(uid, passInfo.second.index);

		for (auto& passRewardState : passRewardStates) {
			int level = passRewardState.level;
			int type = passRewardState.passType;

			playerPassStateList[passIndex].isRewardedList[level][type] = (bool)passRewardState.isRewarded;
		}

		//pServer->GetPacketSender()->SendUserPassStatePacket(player->GetSessionID(), playerPassStateList[passIndex]);
	}

}

void LobbyManager::LoadMissionProgress(Player* player)
{
	int uid = player->GetUID();

	//임시
	int pass_index = 601;

	UserMissionList& playerMissionList = player->GetMissionList();

	playerMissionList.missionList.clear();

	// DB에서 미션 진행상황 불러옴
	std::vector<UserMission> missionListFromDB = pDB->SelectUserMission(uid);

	if (!missionListFromDB.empty()) {
		for (UserMission& mission : missionListFromDB) {

			PassMissionInfo missionInfo = pTableManager->GetPassMissionDataListByIndex()[mission.mission_code];

			int type = missionInfo.type;
			int category = missionInfo.mission_category;
			int group = missionInfo.mission_group;
			int step = missionInfo.mission_step;

			playerMissionList.missionList[pass_index][type][category][group][step] = mission;
		}
	}

	// 진행이 되지 않은 1단계 미션 추가
	int firstStep = 1;

	std::unordered_map<int, PassMissionInfo>& passMissionDataList = pTableManager->GetPassMissionDataListByIndex();

	std::time_t nowTime = std::time(nullptr);
	std::tm tNowTime = {};
	localtime_s(&tNowTime, &nowTime);

	for (auto& missionCategoryList : pTableManager->GetPassMissionIndexList()[pass_index]) {
		int type = missionCategoryList.first;

		for (auto& missionGroupList : missionCategoryList.second) {
			int category = missionGroupList.first;

			for (auto& missionStepList : missionGroupList.second) {
				int group = missionStepList.first;

				int firstMissionIndex = missionStepList.second[firstStep];

				// <Group, <Step, missionInfo>>
				std::unordered_map<int, std::unordered_map<int, UserMission>>& playerMissionGroupList = playerMissionList.missionList[pass_index][type][category];

				if (playerMissionGroupList.find(group) == playerMissionGroupList.end()) {
					PassMissionInfo& newMission = passMissionDataList[firstMissionIndex];
					playerMissionGroupList[group][firstStep].Init(uid, newMission.index, newMission.type
						, newMission.mission_group, newMission.mission_step, newMission.required_count, tNowTime);
				}
			}
		}
	}

	//pServer->GetPacketSender()->SendUserMissionStatePacket(player->GetSessionID(), playerMissionList);
}

bool LobbyManager::UpdateGachaMission(Player* player, int itemCode)
{
	std::vector<UserMission> updatedMissionList;

	// 임시
	int pass_index = 601;

	// 가챠 횟수 미션
	{
		// 일일 미션
		for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_DAILY][eMissionCategory::MC_GACHA]) {

			std::unordered_map<int, UserMission>& missionInfos = missionGroupList.second;

			for (auto& missionInfo : missionInfos) {
				missionInfo.second.progress++;

				updatedMissionList.push_back(missionInfo.second);
			}
		}

		// 패스 미션
		for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_PASS][eMissionCategory::MC_GACHA]) {

			std::unordered_map<int, UserMission>& missionInfos = missionGroupList.second;

			for (auto& missionInfo : missionInfos) {
				missionInfo.second.progress++;

				updatedMissionList.push_back(missionInfo.second);
			}
		}
	}


	std::unordered_map<int, PassMissionInfo>& passMissionInfoList = pTableManager->GetPassMissionDataListByIndex();
	// 아이템 획득 미션
	int itemGrade = (int)pTableManager->GetItemInfos()[itemCode].Item_Grade;

	{
		// 일일 미션
		for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_DAILY][eMissionCategory::MC_GET_ITEM]) {

			std::unordered_map<int, UserMission>& missionInfos = missionGroupList.second;

			for (auto& missionInfo : missionInfos) {

				int required_item_grade = passMissionInfoList[missionInfo.second.mission_code].required_item_grade;

				if (itemGrade == required_item_grade) {
					missionInfo.second.progress++;

					updatedMissionList.push_back(missionInfo.second);
				}
			}
		}

		// 패스 미션
		for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_PASS][eMissionCategory::MC_GET_ITEM]) {

			std::unordered_map<int, UserMission>& missionInfos = missionGroupList.second;

			for (auto& missionInfo : missionInfos) {

				int required_item_grade = passMissionInfoList[missionInfo.second.mission_code].required_item_grade;

				if (itemGrade == required_item_grade) {
					missionInfo.second.progress++;

					updatedMissionList.push_back(missionInfo.second);
				}
			}
		}
	}


	pDB->UpsertUserMission(player->GetUID(), updatedMissionList);

	return true;
}

bool LobbyManager::UpdateLoginMission(Player* player)
{
	//임시
	int pass_index = 601;

	time_t now = std::time(nullptr);
	
	std::tm today{};
	localtime_s(&today, &now);

	std::tm lastLoginDate = player->GetLastLoginTime();

	if (today.tm_year != lastLoginDate.tm_year
		|| today.tm_mon != lastLoginDate.tm_mon
		|| today.tm_mday != lastLoginDate.tm_mday) {

		std::vector<UserMission> updatedMissionList;

		// 출석 미션
		{
			// 일일 미션
			for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_DAILY][eMissionCategory::MC_ATTENDANCE]) {

				std::unordered_map<int, UserMission>& missionInfos = missionGroupList.second;

				for (auto& missionInfo : missionInfos) {
					missionInfo.second.progress++;

					updatedMissionList.push_back(missionInfo.second);
				}
			}

			// 패스 미션
			for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_PASS][eMissionCategory::MC_ATTENDANCE]) {

				std::unordered_map<int, UserMission>& missionInfos = missionGroupList.second;

				for (auto& missionInfo : missionInfos) {
					missionInfo.second.progress++;

					updatedMissionList.push_back(missionInfo.second);
				}
			}
		}

		if (!updatedMissionList.empty()) {
			pDB->UpsertUserMission(player->GetUID(), updatedMissionList);
			player->SetLastLoginTime(today);
		}

		return true;
	}

	return false;
}

bool LobbyManager::CheckCompleteMission(Player* player, int missionCode)
{
	PassMissionInfo& missionInfo = pTableManager->GetPassMissionDataListByIndex()[missionCode];
	std::vector<UserMission> updateMissionList;

	int uid = player->GetUID();
	int nextStep = missionInfo.mission_step + 1;
	int passIndex = missionInfo.pass_index;

	auto& missionIndexList = pTableManager->GetPassMissionIndexList()[passIndex][missionInfo.type][missionInfo.mission_category][missionInfo.mission_group];
	auto& playerMissionList = player->GetMissionList().missionList[passIndex][missionInfo.type][missionInfo.mission_category][missionInfo.mission_group];

	UserMission& currMission = playerMissionList[missionInfo.mission_step];

	// 완료 조건 확인
	if (currMission.progress < missionInfo.required_count) {
		return false;
	}


	// 완료 처리
	std::time_t nowTime = std::time(nullptr);
	std::tm tNowTime = {};
	localtime_s(&tNowTime, &nowTime);

	currMission.is_completed = true;
	currMission.complete_time = tNowTime;
	updateMissionList.push_back(currMission);

	// 패스 보상 지급
	GiveMissionReward(player, missionInfo);


	// 다음 단계 미션 갱신
	auto nextMissioniter = missionIndexList.find(nextStep);

	if (nextMissioniter != missionIndexList.end()) {
		int nextMissionCode = nextMissioniter->second;

		UserMission newMission;

		newMission.Init(player->GetUID(), pTableManager->GetPassMissionDataListByIndex()[nextMissionCode], currMission.progress, tNowTime);

		playerMissionList.insert({nextStep, newMission });

		updateMissionList.push_back(playerMissionList[nextStep]);
	}

	pDB->UpsertUserMission(uid, updateMissionList);

	return true;
}

bool LobbyManager::GiveMissionReward(Player* player, PassMissionInfo& missionInfo)
{
	const int uid = player->GetUID();
	const int passIndex = missionInfo.pass_index;

	PlayerPassInfo& playerPassInfo = player->GetPassInfo()[passIndex];
	
	const int currExp = playerPassInfo.passState.passExp;
	int rewardExp = 0;
	int nextExp = 0;

	// 패스 경험치
	switch (missionInfo.type) {
	case eMissionType::MT_DAILY:
	{
		const int todayDailyMissionExp = playerPassInfo.passState.daily_mission_exp;
		const int limitExpDailyMission = pTableManager->GetPassList()[passIndex].limit_exp_daily_mission;

		// 일일 경험치 한도 체크
		if (todayDailyMissionExp + missionInfo.reward_exp > limitExpDailyMission) {
			rewardExp = limitExpDailyMission - todayDailyMissionExp;
		}
		else {
			rewardExp = missionInfo.reward_exp;
		}

		playerPassInfo.passState.daily_mission_exp += rewardExp;

	}
	break;
	case eMissionType::MT_PASS:
	default:
	{
		rewardExp = missionInfo.reward_exp;
	}
	break;
	}

	nextExp = currExp + rewardExp;
	playerPassInfo.SetExp(nextExp);
	CheckPassLevelUp(playerPassInfo, passIndex);
	pDB->UpsertUserPass(uid, playerPassInfo.passState);

	// 아이템 지급
	std::unordered_map<int, UserItem>& playerItems = player->GetItems();

	const int itemCode = missionInfo.reward_item;
	if (itemCode != 0) {
		ItemTable& rewardInfo = pTableManager->GetItemInfos()[itemCode];
		if (rewardInfo.Item_Type == ItemType::Money) {
			UserItem reward(uid, missionInfo.reward_item, missionInfo.reward_item_amount);
			pDB->UpsertUserCurrency(uid, std::vector<UserItem>{reward});
		}
		else {
			pDB->InsertUserItem(uid, missionInfo.reward_item, missionInfo.reward_item_amount, (int)rewardInfo.Item_Type);
		}

		playerItems[itemCode].owner_UID = uid;
		playerItems[itemCode].itemCode = itemCode;
		playerItems[itemCode].itemType = (int)rewardInfo.Item_Type;
		playerItems[itemCode].count += missionInfo.reward_item_amount;
	}

	return true;
}

bool LobbyManager::CheckPassLevelUp(PlayerPassInfo& playerPassInfo, int passIndex)
{
	const int currExp = playerPassInfo.passState.passExp;
	const int currLevel = playerPassInfo.passState.passLevel;

	const int expLimit = pTableManager->GetPassList()[passIndex].passLevelList[1][ePassType::PT_NORMAL].Exp_Required;

	// 임시
	if (currExp >= expLimit) {

		const int nextExp = currExp % expLimit;
		const int nextLevel = currLevel + (currExp / expLimit);

		playerPassInfo.SetExp(nextExp);
		playerPassInfo.SetLevel(nextLevel);

		return true;
	}

	return false;
}

bool LobbyManager::GivePassReward(Player* player, int pass_index, int pass_type, int level)
{
	PassInfo& passInfo = pTableManager->GetPassList()[pass_index];
	const int uid = player->GetUID();
	const int reward_level = (level > passInfo.level_repeated_reward) ? passInfo.level_repeated_reward : level;
	std::unordered_map<int, UserItem>& playerItems = player->GetItems();

	PlayerPassInfo& playerPassInfo = player->GetPassInfo()[pass_index];

	PassLevel& passLevelInfo = pTableManager->GetPassList()[pass_index].passLevelList[reward_level][pass_type];

	// 수령 가능 여부 체크
	if (pass_type == ePassType::PT_PREMIUM && playerPassInfo.passState.passType != ePassType::PT_PREMIUM) {
		return false;
	}
	if (playerPassInfo.isRewardedList[level][pass_type] == true) {
		return false;
	}
	if (playerPassInfo.passState.passLevel < level) {
		return false;
	}


	// 아이템 지급
	int itemCode = passLevelInfo.Reward_Item_Index;
	if (itemCode != 0) {
		ItemTable& rewardInfo = pTableManager->GetItemInfos()[itemCode];
		if (rewardInfo.Item_Type == ItemType::Money) {
			UserItem reward(uid, itemCode, passLevelInfo.Reward_Item_Amount);
			pDB->UpsertUserCurrency(uid, std::vector<UserItem>{reward});

			pDB->UpsertUserCurrencyRecord(uid, itemCode, passLevelInfo.Reward_Item_Amount, 0);
		}
		else {
			pDB->InsertUserItem(uid, itemCode, passLevelInfo.Reward_Item_Amount, (int)rewardInfo.Item_Type);
		}

		pDB->InsertUserPassReward(uid, passLevelInfo, level);

		playerPassInfo.SetIsRewarded(level, pass_type);

		playerItems[itemCode].owner_UID = uid;
		playerItems[itemCode].itemCode = itemCode;
		playerItems[itemCode].itemType = (int)rewardInfo.Item_Type;
		playerItems[itemCode].count += passLevelInfo.Reward_Item_Amount;

		return true;
	}

	return false;
}

bool LobbyManager::PurchaseShopGoods(Player* player, Shop_Goods& goodsInfo, int goods_index)
{
	int uid = player->GetUID();
	DB* db = pServer->GetDB();

	std::unordered_map<int, UserItem>& playerItems = player->GetItems();
	int item_index = goodsInfo.Item;
	int item_amount = goodsInfo.Item_Amount;
	ItemType item_type = pServer->GetTableManager()->GetItemInfos()[item_index].Item_Type;

	bool result = false;
	int curr_currency = playerItems[goodsInfo.Currency_ID].count;

	// 재화량 확인
	if (curr_currency < goodsInfo.Price) {
		return false;
	}

	// 재화 소모
	const ERROR_CODE payResult = db->UpdateUserItemCount(uid, goodsInfo.Currency_ID, -goodsInfo.Price);
	if (payResult == ERROR_CODE::ER_DB_ERROR || payResult == ERROR_CODE::ER_DB_NO_DATA) {
		return false;
	}
	playerItems[goodsInfo.Currency_ID].count -= goodsInfo.Price;

	const ERROR_CODE insertReceiptResult = db->InsertReceipt(uid, goods_index);

	// 아이템 지급
	switch (item_type) {
	case ItemType::Money: {
		const ERROR_CODE upsertResult = db->UpsertUserItemCount(uid, item_index, item_amount);
		if (upsertResult == ERROR_CODE::ER_DB_ERROR) {
			result = false;
		}
		else {

			pDB->UpsertUserCurrencyRecord(uid, item_index, item_amount, 0);
			result = true;
		}
	}break;
	case ItemType::Skin:
	case ItemType::Emotion:
	case ItemType::Accessory_Back:
	case ItemType::Accessory_Face:
	case ItemType::Accessory_Head: {
		int currItemCount = player->GetItems().count(item_index);
		// 보유한 아이템이 아닌 경우
		if (currItemCount == 0) {
			const ERROR_CODE insertResult = db->InsertUserItem(uid, item_index, item_amount, (int)item_type);
			if (insertResult == ERROR_CODE::ER_DB_ERROR) {
				result = false;
			}
			else {
				result = true;
			}
		}
	}break;
	default: {

	}break;
	}

	// 구매 성공
	if (result == true) {
		playerItems[item_index].owner_UID = uid;
		playerItems[item_index].itemCode = item_index;
		playerItems[item_index].itemType = (int)item_type;
		playerItems[item_index].count = item_amount;

		pDB->UpsertUserCurrencyRecord(uid, goodsInfo.Currency_ID, 0, goodsInfo.Price);

		return true;
	}
	// 구매 실패
	else {
		db->UpdateUserItemCount(uid, goodsInfo.Currency_ID, goodsInfo.Price);
		playerItems[goodsInfo.Currency_ID].count += goodsInfo.Price;
		return false;
	}
}


bool LobbyManager::PurchasePass(Player* player, Shop_Goods& goodsInfo, int goods_index)
{
	int uid = player->GetUID();
	DB* db = pServer->GetDB();

	int pass_index = goodsInfo.Item;

	std::unordered_map<int, UserItem>& playerItems = player->GetItems();
	UserPass& passState = player->GetPassInfo()[pass_index].passState;
	int curr_currency = playerItems[goodsInfo.Currency_ID].count;

	// 패스 등급 확인
	if (passState.passType == ePassType::PT_PREMIUM) {
		return false;
	}

	// 재화량 확인
	if (curr_currency < goodsInfo.Price) {
		return false;
	}

	// 재화 소모
	const ERROR_CODE payResult = db->UpdateUserItemCount(uid, goodsInfo.Currency_ID, -goodsInfo.Price);
	if (payResult == ERROR_CODE::ER_DB_ERROR || payResult == ERROR_CODE::ER_DB_NO_DATA) {
		return false;
	}

	pDB->UpsertUserCurrencyRecord(uid, goodsInfo.Currency_ID, 0, goodsInfo.Price);

	playerItems[goodsInfo.Currency_ID].count -= goodsInfo.Price;
	curr_currency = playerItems[goodsInfo.Currency_ID].count;

	const ERROR_CODE insertReceiptResult = db->InsertReceipt(uid, goods_index);

	// 패스 업그레이드
	passState.passType = ePassType::PT_PREMIUM;

	ERROR_CODE updatePassResult = pDB->UpsertUserPass(uid, passState);

	if (updatePassResult == ERROR_CODE::ER_NONE) {
		return true;
	} 
	else {
		return false;
	}

}