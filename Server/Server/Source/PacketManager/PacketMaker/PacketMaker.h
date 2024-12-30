#pragma once
#include "../../utility.h"
#include "../../protocol.h"
#include "../flatbuffer/FlatBufferManager.h"
#include "../../DB/Schema/Schemas.h"
#include "../../TableManager/Tables/Tables.h"

#include <unordered_map>
#include <array>

class PacketMaker
{
public:
	PacketMaker() {};
	~PacketMaker() {};

	std::vector<uint8_t> MakeVersionCheckResponsePacket(int result);
	std::vector<uint8_t> MakeLoginResponsePacket(int result, UserInfo userInfo, std::unordered_map<int, std::set<sDayAttendanceInfo>>& attendanceInfoList, bool isNewEvent);
	std::vector<uint8_t> MakeSignUpResponsePacket(int result);

	std::vector<uint8_t> MakeGachaResponsePacket(int result, GachaItem& result_item, GachaItem& acquired_item, int spent_resource_type, int spent_resource_amount, int remaining_resource_amount);
	std::vector<uint8_t> MakeCurrencyAmountResponsePacket(int result, std::vector<UserItem>& currency_list);
	std::vector<uint8_t> MakeUserItemsResponsePacket(int result, std::unordered_map<int, UserItem>& user_items);
	std::vector<uint8_t> MakeAttendanceEventResponsePacket(int eventCode, int dayCount, int result);
	std::vector<uint8_t> MakeAttendanceRewardResponsePacket(int eventCode, int day_count, int result, int reward_item, int reward_item_count);

	std::vector<uint8_t> MakePlayerAddPacket(std::vector<class Player*>& players);
	std::vector<uint8_t> MakePlayerDeletePacket (int inGameID);
	std::vector<uint8_t> MakePlayerDeadPacket(int inGameID, int roomID, Vector3f position, Vector3f direction, int spawn_delay);
	std::vector<uint8_t> MakePlayerRespawnPacket(int inGameID, int roomID, Vector3f position, int hp);
	std::vector<uint8_t> MakePlayerCalculatedDamagePacket(int targetID, int attackType, int hp, int damageAmount, int targetStaminaLoss, Vector3f knockback_direction);
	std::vector<uint8_t> MakePlayerGroggyPacket(int id, int playerHP);
	std::vector<uint8_t> MakePlayerGroggyRecoveryPacket(int id, int recoveredStamina);
	std::vector<uint8_t> MakePlayerThrowOtherPlayerPacket(int playerID, Vector3f myPosition, Vector3f myDirection, int targetID, Vector3f targetPosition, Vector3f targetDirection);

	std::vector<uint8_t> MakeGameMatchingResponsePacket(int inGameID, sMatchingInfo matchingInfo, std::vector<class Player*> players);
	std::vector<uint8_t> MakeAllPlayerReadyPacket();
	std::vector<uint8_t> MakeGameStartPacket(int roomID, long time = 0);

	std::vector<uint8_t> MakeHeartBeatPacket();
	std::vector<uint8_t> MakeBlockDropPacket(std::vector<std::pair<int, int>>& positions, std::vector<int>& blockTypes);
	std::vector<uint8_t> MakeBombSpawnPacket(std::vector<Vector3f>& Positions, std::vector<int>& bombIDs, int explosionInterval);
	std::vector<uint8_t> MakeBombDropPacket(Vector3f position, int roomID, int bombID);
	std::vector<uint8_t> MakeWeaponSpawnPacket(std::vector<Vector3f>& positions, std::vector<int>& weaponIDs, std::vector<int>& weaponTypes);
	std::vector<uint8_t> MakeWeaponDropPacket(Vector3f position, int weaponid);
	std::vector<uint8_t> MakeBombExplosionPacket(int bombID, Vector3f position);
	std::vector<uint8_t> MakeGameStartPacket(int roomID, int startTime);
	std::vector<uint8_t> MakeGameEndPacket(uint8_t winningTeams_flag);
	std::vector<uint8_t> MakeGameResultPacket(std::set<int>& winningTeams, std::unordered_map<int, sPlayerGameRecord>& records, std::unordered_map<int, class Player*>& players);
	std::vector<uint8_t> MakeGameHostChangePacket(int inGameID, int roomID);

	std::vector<uint8_t> MakeLifeReducePacket(int team, int lifeCount);
	std::vector<uint8_t> MakeRemainTimeSyncPacket(int roomID, TIMEPOINT time, int gameEndTime);

private:

};

