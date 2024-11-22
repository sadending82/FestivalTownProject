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

void LobbyManager::CheckAttendanceEvent(int uid, std::unordered_map<int, std::vector<sDayAttendanceInfo>>& attendanceInfoList)
{
	std::time_t NowTime = std::time(nullptr);
	for (auto& eventInfo : pTableManager->GetEventList()) {

		int eventCode = eventInfo.first;

		std::time_t openTime = std::mktime(const_cast<std::tm*>(&eventInfo.second.Open_Date));
		std::time_t closeTime = std::mktime(const_cast<std::tm*>(&eventInfo.second.Close_Date));

		if (NowTime < openTime || closeTime < NowTime) {
			continue;
		}

		if (eventInfo.second.Type == 2) {
			attendanceInfoList[eventCode] = pDB->SelectUserAttendanceEvent(uid, eventCode);
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

	int payResult = pDB->UpdateUserItemCount(uid, payItem, -price);

	if (payResult == false) {
		return false;
	}

	// 재화나 티켓인 경우
	if (itemType == (int)ItemType::Money) {
		bool result = pDB->UpsertUserItemCount(uid, gachaItem.Reward_Item_Index, gachaItem.Reward_Item_Value);
		if (result == false) {
			// 실패 시 재화 다시 돌려주고 실패 반환
			pDB->UpdateUserItemCount(uid, payItem, price);
			return false;
		}
	}
	else {
		int currItemCount = pDB->SelectUserItemCount(uid, gachaItem.Reward_Item_Index);
		// 새로 얻은 경우
		if (currItemCount == 0) {
			bool result = pDB->InsertUserItem(uid, gachaItem.Reward_Item_Index, gachaItem.Reward_Item_Value, itemType);
			if (result == false) {
				// 실패 시 재화 다시 돌려주고 실패 반환
				pDB->UpdateUserItemCount(uid, payItem, price);
				return false;
			}
		}
		// 이미 있는 경우
		else {
			// 마일리지 지급
			int itemGrade = (int)pTableManager->GetItemInfos()[gachaItem.Reward_Item_Index].Item_Grade;
			int mileage = pTableManager->GetGachaAcquiredMileages()[itemGrade];
			bool result = pDB->UpsertUserItemCount(uid, mileageIndex, mileage);
			if (result == false) {
				// 실패 시 재화 다시 돌려주고 실패 반환
				pDB->UpdateUserItemCount(uid, payItem, price);
				return false;
			}

			gachaItem.Reward_Item_Index = mileageIndex;
			gachaItem.Reward_Item_Value = mileage;
		}
	}

	return true;
}

