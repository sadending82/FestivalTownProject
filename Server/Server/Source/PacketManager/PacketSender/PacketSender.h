#pragma once
#include "../PacketMaker/PacketMaker.h"

class PacketSender
{
public:
	PacketSender(class Server* server, class PacketMaker* packetMaker) :mServer(server), mPacketMaker(packetMaker) {}
	~PacketSender();

	void SendVersionCheckResponsePacket(int sessionID, int result);
	void SendLoginResponse(int sessionID, int result, UserInfo& userInfo, std::unordered_map<int, UserItem>& items, std::unordered_map<int, std::set<sDayAttendanceInfo>>& attendanceInfoList, bool isNewEvent);
	void SendSignUpResponse(int sessionID, int result);

	void SendGachaResponsePacket(int sessionID, int result, GachaItem& result_item, GachaItem& acquired_item, int spent_resource_type, int spent_resource_amount, int remaining_resource_amount);
	void SendCurrencyAmountResponsePacket(int sessionID, int result, std::unordered_map<int, UserItem>& currency_list);
	void SendUserItemsResponsePacket(int sessionID, int result, std::unordered_map<int, UserItem>& user_items);
	void SendAttendanceEventResponsePacket(int sessionID, int eventCode, int dayCount, int result);
	void SendAttendanceRewardResponsePacket(int sessionID, int eventCode, int day_count, int result, int reward_item, int reward_item_count);
	void SendEventRewardResponsePacket(int sessionID, int eventCode, int result, int reward_item, int reward_item_count);
	void SendUserPassStatePacket(int sessionID, PlayerPassInfo& playerPassState);
	void SendUserMissionStatePacket(int sessionID, UserMissionList& playerMissionState);
	void SendMissionCompleteResponsePacket(int sessionID, int result, PlayerPassInfo& playerPassInfo, UserMission& missionState, int reward_item_index, int reward_item_amount);
	void SendPassRewardResponsePacket(int sessionID, int result, int pass_index, int pass_type, int reward_level, int reward_item_index, int reward_item_amount);
	void SendBatchReceivePassRewardsResponsePacket(int sessionID, int pass_index, std::vector<ReceivedPassReward>& rewards);
	void SendPurchaseGoodsResponsePacket(int sessionID, int result, int goods_index, int currency_index, int currency_amount);

	void SendPlayerAdd(int roomID);
	void SendPlayerDelete(int roomID, int inGameID);
	void SendGameMatchingResponse(int sessionID, int roomID);
	void SendGameStart(int roomID);
	void SendAllPlayerReady(int roomID);

	void SendHeartBeatPacket(int sessionID);
	void SendBlockDropPacket(int roomID, int spawnCount, int blockType);
	void SendBombSpawnPacket(std::vector<Vector3f>& Positions, std::vector<int>& bombIDs, int explosionInterval, int roomID);
	void SendBombExplosionPacket(int roomID, int bombID);
	void SendLifeReducePacket(int team, int lifeCount, int roomID);
	void SendRemainTimeSync(int roomID);

	void SendGameStartPacket(int roomID);
	void SendGameEndPacket(int roomID, uint8_t winningTeams_flag);
	void SendGameResultPacket(int roomID, std::set<int>& winningTeams);

	void SendGameHostChange(int sessionID);
	void SendPlayerDeadPacket(int inGameID, int roomID, int spawn_delay);
	void SendPlayerRespawn(int inGameID, int roomID, Vector3f spawnPos);
	void SendBombDropPacket(Vector3f position, int roomID, int bombID);
	void SendWeaponSpawnPacket(std::vector<Vector3f>& positions, std::vector<int>& weaponIDs, std::vector<int>& weaponTypes, int roomID);
	void SendWeaponDropPacket(Vector3f position, int roomID, int weaponID);
	void SendPlayerCalculatedDamage(int targetID, int roomID, int attackType, int hp, int damageAmount, int stamina, Vector3f knockback_direction);
	void SendPlayerGroggyPacket(int inGameID, int roomID, int playerHP);
	void SendPlayerGroggyRecoveryPacket(int inGameID, int roomID, int recoveredStamina);
	void SendPlayerThrowOtherPlayerPacket(int roomID, int inGameID, Vector3f myPosition, Vector3f myDirection, int targetID, Vector3f targetPosition, Vector3f targetDirection);

private:
	class Server* mServer = nullptr;
	class PacketMaker* mPacketMaker = nullptr;
};