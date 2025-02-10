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

				// ���� �⼮ �������� �⼮ ó��
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

bool LobbyManager::GiveGachaItemToUser(int uid, int payItem, int price, GachaItem& gachaItem)
{
	int itemType = (int)pTableManager->GetItemInfos()[gachaItem.Reward_Item_Index].Item_Type;
	int mileageIndex = 100003;

	ERROR_CODE payResult = pDB->UpdateUserItemCount(uid, payItem, -price);

	if (payResult == ERROR_CODE::ER_DB_ERROR || payResult == ERROR_CODE::ER_DB_NO_DATA) {
		return false;
	}

	// ��ȭ�� Ƽ���� ���
	if (itemType == (int)ItemType::Money) {
		ERROR_CODE result = pDB->UpsertUserItemCount(uid, gachaItem.Reward_Item_Index, gachaItem.Reward_Item_Value);
		if (result == ERROR_CODE::ER_DB_ERROR) {
			// ���� �� ��ȭ �ٽ� �����ְ� ���� ��ȯ
			pDB->UpdateUserItemCount(uid, payItem, price);
			return false;
		}
	}
	else {
		int currItemCount = pDB->SelectUserItemCount(uid, gachaItem.Reward_Item_Index);
		// ���� ���� ���
		if (currItemCount == 0) {
			ERROR_CODE result = pDB->InsertUserItem(uid, gachaItem.Reward_Item_Index, gachaItem.Reward_Item_Value, itemType);
			if (result == ERROR_CODE::ER_DB_ERROR) {
				// ���� �� ��ȭ �ٽ� �����ְ� ���� ��ȯ
				pDB->UpdateUserItemCount(uid, payItem, price);
				return false;
			}
		}
		// �̹� �ִ� ���
		else {
			// ���ϸ��� ����
			int itemGrade = (int)pTableManager->GetItemInfos()[gachaItem.Reward_Item_Index].Item_Grade;
			int mileage = pTableManager->GetGachaAcquiredMileages()[itemGrade];
			ERROR_CODE result = pDB->UpsertUserItemCount(uid, mileageIndex, mileage);
			if (result == ERROR_CODE::ER_DB_ERROR) {
				// ���� �� ��ȭ �ٽ� �����ְ� ���� ��ȯ
				pDB->UpdateUserItemCount(uid, payItem, price);
				return false;
			}

			gachaItem.Reward_Item_Index = mileageIndex;
			gachaItem.Reward_Item_Value = mileage;
		}
	}

	return true;
}

void LobbyManager::LoadPassState(Player* player)
{
	int uid = player->GetUID();

	std::unordered_map<int, PlayerPassInfo>& playerPassStateList = player->GetPassInfo();

	std::unordered_map<int, PassInfo>& passList = pTableManager->GetPassList();

	std::time_t nowTime = std::time(nullptr);

	for (auto& passInfo : passList) {

		int passIndex = passInfo.second.index;

		std::time_t openTime = std::mktime(const_cast<std::tm*>(&passInfo.second.open_date));
		std::time_t closeTime = std::mktime(const_cast<std::tm*>(&passInfo.second.close_date));

		if (nowTime < openTime || closeTime < nowTime) {
			continue;
		}

		UserPass passState = pDB->SelectUserPass(uid, passInfo.second.index);

		playerPassStateList[passIndex].Init(passState);

		std::unordered_map<int, UserPassReward> passRewardStates = pDB->SelectUserPassReward(uid, passInfo.second.index);

		for (auto& passRewardState : passRewardStates) {
			int level = passRewardState.second.level;
			int type = passRewardState.second.passType;

			playerPassStateList[passIndex].isRewardedList[level][type] = (bool)passRewardState.second.isRewarded;
		}

		//pServer->GetPacketSender()->SendUserPassStatePacket(player->GetSessionID(), playerPassStateList[passIndex]);
	}

}

void LobbyManager::LoadMissionProgress(Player* player)
{
	int uid = player->GetUID();

	//�ӽ�
	int pass_index = 601;

	UserMissionList& playerMissionList = player->GetMissionList();

	// DB���� �̼� �����Ȳ �ҷ���
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

	// ������ ���� ���� 1�ܰ� �̼� �߰�
	int firstStep = 1;

	std::unordered_map<int, PassMissionInfo>& passMissionDataList = pTableManager->GetPassMissionDataListByIndex();

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
					UserMission& playerMission = playerMissionGroupList[group][firstStep];
					PassMissionInfo& newMission = passMissionDataList[firstMissionIndex];
					playerMission.Init(uid, newMission.index, newMission.type, newMission.mission_group, newMission.mission_step, newMission.required_count);
				}
			}
		}
	}

	//pServer->GetPacketSender()->SendUserMissionStatePacket(player->GetSessionID(), playerMissionList);
}

bool LobbyManager::UpdateGachaMission(Player* player, int itemCode)
{
	std::vector<UserMission> updatedMissionList;

	// �ӽ�
	int pass_index = 601;

	// ��í Ƚ�� �̼�
	{
		// ���� �̼�
		for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_DAILY][eMissionCategory::MC_GACHA]) {

			std::unordered_map<int, UserMission> missionInfos = missionGroupList.second;

			for (auto& missionInfo : missionInfos) {
				missionInfo.second.progress++;

				updatedMissionList.push_back(missionInfo.second);
			}
		}

		// �н� �̼�
		for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_PASS][eMissionCategory::MC_GACHA]) {

			std::unordered_map<int, UserMission> missionInfos = missionGroupList.second;

			for (auto& missionInfo : missionInfos) {
				missionInfo.second.progress++;

				updatedMissionList.push_back(missionInfo.second);
			}
		}
	}


	std::unordered_map<int, PassMissionInfo>& passMissionInfoList = pTableManager->GetPassMissionDataListByIndex();
	// ������ ȹ�� �̼�
	int itemGrade = (int)pTableManager->GetItemInfos()[itemCode].Item_Grade;

	{
		// ���� �̼�
		for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_DAILY][eMissionCategory::MC_GET_ITEM]) {

			std::unordered_map<int, UserMission> missionInfos = missionGroupList.second;

			for (auto& missionInfo : missionInfos) {

				int required_item_grade = passMissionInfoList[missionInfo.second.mission_code].required_item_grade;

				if (itemGrade == required_item_grade) {
					missionInfo.second.progress++;

					updatedMissionList.push_back(missionInfo.second);
				}
			}
		}

		// �н� �̼�
		for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_PASS][eMissionCategory::MC_GET_ITEM]) {

			std::unordered_map<int, UserMission> missionInfos = missionGroupList.second;

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
	//�ӽ�
	int pass_index = 601;

	time_t now = std::time(nullptr);
	
	std::tm today{};
	localtime_s(&today, &now);

	std::tm lastLoginDate = player->GetLastLoginTime();

	if (today.tm_year != lastLoginDate.tm_year
		|| today.tm_mon != lastLoginDate.tm_mon
		|| today.tm_mday != lastLoginDate.tm_mday) {

		std::vector<UserMission> updatedMissionList;

		// �⼮ �̼�
		{
			// ���� �̼�
			for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_DAILY][eMissionCategory::MC_ATTENDANCE]) {

				std::unordered_map<int, UserMission> missionInfos = missionGroupList.second;

				for (auto& missionInfo : missionInfos) {
					missionInfo.second.progress++;

					updatedMissionList.push_back(missionInfo.second);
				}
			}

			// �н� �̼�
			for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_PASS][eMissionCategory::MC_ATTENDANCE]) {

				std::unordered_map<int, UserMission> missionInfos = missionGroupList.second;

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

	// �Ϸ� ���� Ȯ��
	if (currMission.progress < missionInfo.required_count) {
		return false;
	}

	currMission.is_rewarded = true;

	updateMissionList.push_back(currMission);

	// �н� ���� ���� ��� �߰� �ʿ�



	// ���� �ܰ� �̼� ����
	auto nextMissioniter = missionIndexList.find(nextStep);

	if (nextMissioniter != missionIndexList.end()) {
		int nextMissionCode = nextMissioniter->second;

		UserMission newMission;
		newMission.Init(player->GetUID(), pTableManager->GetPassMissionDataListByIndex()[nextMissionCode]);

		playerMissionList.insert({nextStep, newMission });

		updateMissionList.push_back(playerMissionList[nextStep]);
	}

	pDB->UpsertUserMission(uid, updateMissionList);

	return true;
}

bool LobbyManager::GiveMissionReward(Player* player, PassMissionInfo& missionInfo)
{
	int uid = player->GetUID();
	// �ӽ�
	int passIndex = 601;

	PlayerPassInfo& playerPassInfo = player->GetPassInfo()[passIndex];

	// �н� ����ġ
	playerPassInfo.SetExp(missionInfo.reward_exp);
	pDB->UpsertUserPass(uid, playerPassInfo.passState);
	CheckPassLevelUp(player, playerPassInfo);

	// ������ ����
	int itemCode = missionInfo.reward_item;
	if (itemCode != 0) {
		ItemTable& rewardInfo = pTableManager->GetItemInfos()[itemCode];
		if (rewardInfo.Item_Type == ItemType::Money) {
			UserItem reward(uid, missionInfo.reward_item, missionInfo.reward_item_amount);
			pDB->UpsertUserCurrency(uid, std::vector<UserItem>{reward});
		}
		else {
			// ���߿� �߰�
		}
	}

	return true;
}

bool LobbyManager::CheckPassLevelUp(Player* player, PlayerPassInfo& playerPassInfo)
{
	int currExp = playerPassInfo.passState.passExp;
	int currLevel = playerPassInfo.passState.passLevel;

	int expLimit = 100;

	// �ӽ�
	if (currExp >= expLimit) {
		playerPassInfo.SetExp(currExp - expLimit);
		playerPassInfo.SetLevel(currLevel + 1);

		pDB->UpsertUserPass(player->GetUID(), playerPassInfo.passState);

		return true;
	}

	return false;
}
