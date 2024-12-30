#pragma once
#include "PacketMaker.h"
#include "../../Object/Player.h"

std::vector<uint8_t> PacketMaker::MakeVersionCheckResponsePacket(int result)
{
	flatbuffers::FlatBufferBuilder Builder;

	Builder.Finish(PacketTable::LoginTable::CreateVersionCheckResponse(Builder, result));
	return MakeBuffer(ePacketType::S2C_VERSION_CHECK_RESPONSE, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeLoginResponsePacket(int result, UserInfo userInfo, std::unordered_map<int, std::set<sDayAttendanceInfo>>& attendanceInfoList, bool isNewEvent)
{
	flatbuffers::FlatBufferBuilder Builder;

	// 캐릭터 커마
	std::vector<flatbuffers::Offset<PacketTable::UtilitiesTable::CustomizingItem>> itemVector;
	for (auto pair : userInfo.characterCustomizing.customizingItems) {
		auto item = pair.second;
		itemVector.push_back(PacketTable::UtilitiesTable::CreateCustomizingItem(Builder, item.itemType, item.item_UID, item.itemCode));
	}
	auto characterCustomizing = PacketTable::UtilitiesTable::CreateCharacterCustomizing(Builder, Builder.CreateVector(itemVector));

	// 출석 이벤트
	std::vector<flatbuffers::Offset<PacketTable::UtilitiesTable::AttendanceStatus>> attendanceStatusVector;
	for (auto& pair : attendanceInfoList) {
		int eventCode = pair.first;
		auto& dayAttendanceList = pair.second;
		bool has_attendanceToday = false;
		std::vector<flatbuffers::Offset<PacketTable::UtilitiesTable::DayAttendanceInfo>> dayAttendanceVector;
		for (auto& attendanceInfo : dayAttendanceList) {
			std::tm tDate = attendanceInfo.attendance_date;

			has_attendanceToday = CheckDateToday(tDate);

			auto date = PacketTable::UtilitiesTable::CreateDate(Builder,tDate.tm_year, tDate.tm_mon, tDate.tm_mday);
			dayAttendanceVector.push_back(PacketTable::UtilitiesTable::CreateDayAttendanceInfo(Builder, attendanceInfo.day_number, date, attendanceInfo.is_rewarded));
		}
		attendanceStatusVector.push_back(PacketTable::UtilitiesTable::CreateAttendanceStatus(Builder, eventCode, has_attendanceToday, Builder.CreateVector(dayAttendanceVector)));
	}

	auto db_userInfo = PacketTable::UtilitiesTable::CreateDB_UserInfo(Builder, userInfo.UID, Builder.CreateString(wstringToString(userInfo.NickName)), userInfo.UserLevel, userInfo.PassLevel
		,  userInfo.Point, userInfo.AttendanceDay, characterCustomizing);

	Builder.Finish(PacketTable::LoginTable::CreateLoginResponse(Builder, result, db_userInfo
		, userInfo.Gold, userInfo.Dia, userInfo.Mileage, isNewEvent, Builder.CreateVector(attendanceStatusVector)));
	return MakeBuffer(ePacketType::S2C_LOGIN_RESPONSE, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeSignUpResponsePacket(int result)
{
	flatbuffers::FlatBufferBuilder Builder;

	Builder.Finish(PacketTable::LoginTable::CreateSignUpResponse(Builder, result));
	return MakeBuffer(ePacketType::S2C_SIGNUP_RESPONSE, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeGachaResponsePacket(int result, GachaItem& result_item, GachaItem& acquired_item, int spent_resource_type, int spent_resource_amount, int remaining_resource_amount)
{
	flatbuffers::FlatBufferBuilder Builder;

	Builder.Finish(PacketTable::LobbyTable::CreateGachaResponse(Builder, result, result_item.Reward_Item_Index, result_item.Reward_Item_Value
		, acquired_item.Reward_Item_Index, acquired_item.Reward_Item_Value, spent_resource_type, spent_resource_amount, remaining_resource_amount));
	return MakeBuffer(ePacketType::S2C_GACHA_RESPONSE, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeCurrencyAmountResponsePacket(int result, std::vector<UserItem>& currency_list)
{
	flatbuffers::FlatBufferBuilder Builder;

	std::vector<int> currency_codes;
	std::vector<int> currency_amounts;

	for (int i = 0; i < currency_list.size(); ++i) {
		currency_codes.push_back(currency_list[i].itemCode);
		currency_amounts.push_back(currency_list[i].count);
	}

	auto codes = Builder.CreateVector(currency_codes);
	auto amounts = Builder.CreateVector(currency_amounts);
	Builder.Finish(PacketTable::LobbyTable::CreateCurrencyAmountResponse(Builder, result, codes, amounts));
	return MakeBuffer(ePacketType::S2C_CURRENCY_AMOUNT_RESPONSE, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeUserItemsResponsePacket(int result, std::unordered_map<int, UserItem>& user_items)
{
	flatbuffers::FlatBufferBuilder Builder;

	std::vector<flatbuffers::Offset<PacketTable::UtilitiesTable::ItemInfo>> item_vector;

	for (auto& pair : user_items) {
		UserItem item = pair.second;
		auto item_info = PacketTable::UtilitiesTable::CreateItemInfo(Builder, item.item_UID, item.owner_UID, item.itemCode, item.count, item.itemType);
		item_vector.push_back(item_info);
	}

	Builder.Finish(PacketTable::LobbyTable::CreateUserItemsResponse(Builder, result, Builder.CreateVector(item_vector)));
	return MakeBuffer(ePacketType::S2C_USER_ITEMS_RESPONSE, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeAttendanceEventResponsePacket(int eventCode, int dayCount, int result)
{
	flatbuffers::FlatBufferBuilder Builder;

	Builder.Finish(PacketTable::LobbyTable::CreateAttendanceEventResponse(Builder, eventCode, result));
	return MakeBuffer(ePacketType::S2C_ATTENDANCE_EVENT_RESPONSE, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeAttendanceRewardResponsePacket(int eventCode, int day_count, int result, int reward_item, int reward_item_count)
{
	flatbuffers::FlatBufferBuilder Builder;

	Builder.Finish(PacketTable::LobbyTable::CreateAttendanceRewardResponse(Builder, eventCode, day_count, result, reward_item, reward_item_count));
	return MakeBuffer(ePacketType::S2C_ATTENDANCE_REWARD_RESPONSE, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakePlayerAddPacket(std::vector<class Player*>& players)
{
	flatbuffers::FlatBufferBuilder Builder;
	std::vector<flatbuffers::Offset<PacketTable::PlayerTable::PlayerInfo>> player_vec;

	for (Player* player : players) {
		if (player == nullptr) {
			continue;
		}
		Vector3f position = player->GetPosition();
		auto pos = PacketTable::UtilitiesTable::CreateVec3f(Builder, position.x, position.y, position.z);
		auto dir = PacketTable::UtilitiesTable::CreateVec3f(Builder, 0, 0, 0);

		std::vector<flatbuffers::Offset<PacketTable::UtilitiesTable::CustomizingItem>> customizingItems;

		for (auto pair : player->GetCharacterCustomizing().customizingItems) {
			auto itemInfo = pair.second;
			auto fItem = PacketTable::UtilitiesTable::CreateCustomizingItem(Builder, itemInfo.itemType, itemInfo.item_UID, itemInfo.itemCode);
			customizingItems.push_back(fItem);
		}

		auto customizingInfo = PacketTable::UtilitiesTable::CreateCharacterCustomizing(Builder, Builder.CreateVector(customizingItems));

		auto pInfo = PacketTable::PlayerTable::CreatePlayerInfo(Builder
			, player->GetInGameID()
			, pos
			, dir
			, player->GetTeam()
			, player->GetCharacterType()
			, customizingInfo
			, Builder.CreateString(wstringToString(player->GetNickName()))
		);

		player_vec.push_back(pInfo);
	}

	Builder.Finish(PacketTable::PlayerTable::CreatePlayerAdd (Builder, Builder.CreateVector(player_vec)));
	return MakeBuffer(ePacketType::S2C_PLAYER_ADD, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakePlayerDeletePacket(int inGameID)
{
	flatbuffers::FlatBufferBuilder Builder;

	Builder.Finish(PacketTable::PlayerTable::CreatePlayerDelete(Builder, inGameID));
	return MakeBuffer(ePacketType::S2C_PLAYER_DELETE, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakePlayerDeadPacket(int inGameID, int roomID, Vector3f position, Vector3f direction, int spawn_delay)
{
	flatbuffers::FlatBufferBuilder Builder;
	
	auto pos = PacketTable::UtilitiesTable::CreateVec3f(Builder, position.x, position.y, position.z);
	auto dir = PacketTable::UtilitiesTable::CreateVec3f(Builder, direction.x, direction.y, direction.z);
	Builder.Finish(PacketTable::PlayerTable::CreatePlayerDead(Builder, inGameID, pos, dir, spawn_delay));
	return MakeBuffer(ePacketType::S2C_PLAYER_DEAD, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakePlayerRespawnPacket(int inGameID, int roomID, Vector3f position, int hp)
{
	flatbuffers::FlatBufferBuilder Builder;
	
	auto pos = PacketTable::UtilitiesTable::CreateVec3f(Builder, position.x, position.y, position.z);
	Builder.Finish(PacketTable::PlayerTable::CreatePlayerRespawn(Builder, inGameID, hp, pos));
	return MakeBuffer(ePacketType::S2C_PLAYER_RESPAWN, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakePlayerCalculatedDamagePacket(int targetID, int attackType, int hp, int damageAmount, int targetStaminaLoss, Vector3f knockback_direction)
{
	flatbuffers::FlatBufferBuilder Builder;
	
	auto dir = PacketTable::UtilitiesTable::CreateVec3f(Builder, knockback_direction.x, knockback_direction.y, knockback_direction.z);
	Builder.Finish(PacketTable::PlayerTable::CreatePlayerCalculatedDamage(Builder, targetID, attackType, hp, damageAmount, targetStaminaLoss, dir));
	return MakeBuffer(ePacketType::S2C_PLAYER_CALCULATED_DAMAGE, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakePlayerGroggyPacket(int id, int playerHP)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Finish(PacketTable::PlayerTable::CreatePlayerGroggy(Builder, id, playerHP));
	return MakeBuffer(ePacketType::S2C_PLAYER_GROGGY, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakePlayerGroggyRecoveryPacket(int id, int recoveredStamina)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Finish(PacketTable::PlayerTable::CreatePlayerGroggyRecovery(Builder, id, recoveredStamina));
	return MakeBuffer(ePacketType::S2C_PLAYER_GROGGY_RECOVERY, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakePlayerThrowOtherPlayerPacket(int playerID, Vector3f myPosition, Vector3f myDirection, int targetID, Vector3f targetPosition, Vector3f targetDirection)
{
	flatbuffers::FlatBufferBuilder Builder;
	auto pos = PacketTable::UtilitiesTable::CreateVec3f(Builder, myPosition.x, myPosition.y, myPosition.z);
	auto dir = PacketTable::UtilitiesTable::CreateVec3f(Builder, myDirection.x, myDirection.y, myDirection.z);

	auto targetPos = PacketTable::UtilitiesTable::CreateVec3f(Builder, targetPosition.x, targetPosition.y, targetPosition.z);
	auto targetDir = PacketTable::UtilitiesTable::CreateVec3f(Builder, targetDirection.x, targetDirection.y, targetDirection.z);

	Builder.Finish(PacketTable::PlayerTable::CreatePlayerThrowOtherPlayer(Builder, playerID, pos, dir, targetID, targetPos, targetDir));
	return MakeBuffer(ePacketType::S2C_PLAYER_THROW_OTHER_PLAYER, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeGameMatchingResponsePacket(int inGameID, sMatchingInfo matchingInfo, std::vector<class Player*> players)
{
	flatbuffers::FlatBufferBuilder Builder;
	
	std::vector<flatbuffers::Offset<PacketTable::PlayerTable::PlayerInfo>> player_vec;

	for (Player* player : players) {
		if (player == nullptr) {
			continue;
		}
		Vector3f position = player->GetPosition();
		auto pos = PacketTable::UtilitiesTable::CreateVec3f(Builder, position.x, position.y, position.z);
		auto dir = PacketTable::UtilitiesTable::CreateVec3f(Builder, 0, 0, 0);

		std::vector<flatbuffers::Offset<PacketTable::UtilitiesTable::CustomizingItem>> customizingItems;

		for (auto pair : player->GetCharacterCustomizing().customizingItems) {
			auto itemInfo = pair.second;
			auto fItem = PacketTable::UtilitiesTable::CreateCustomizingItem(Builder, itemInfo.itemType, itemInfo.item_UID, itemInfo.itemCode);
			customizingItems.push_back(fItem);
		}

		auto customizingInfo = PacketTable::UtilitiesTable::CreateCharacterCustomizing(Builder, Builder.CreateVector(customizingItems));

		auto pInfo = PacketTable::PlayerTable::CreatePlayerInfo(Builder
			, player->GetInGameID()
			, pos
			, dir
			, player->GetTeam()
			, player->GetCharacterType()
			, customizingInfo
			, Builder.CreateString(wstringToString(player->GetNickName()))
		);

		player_vec.push_back(pInfo);
	}

	Builder.Finish(PacketTable::LobbyTable::CreateGameMatchingResponse(Builder, inGameID, matchingInfo.RoomID, matchingInfo.Team, matchingInfo.GameMode, matchingInfo.MapIndex
		, matchingInfo.MapTheme, matchingInfo.GameTime, matchingInfo.IsHost, matchingInfo.TotalPlayerCount, Builder.CreateVector(player_vec)));
	return MakeBuffer(ePacketType::S2C_MATCHING_RESPONSE, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeAllPlayerReadyPacket()
{
	flatbuffers::FlatBufferBuilder Builder;
	
	Builder.Finish(PacketTable::GameTable::CreateAllPlayerReady(Builder));
	return MakeBuffer(ePacketType::S2C_ALL_PLAYER_READY, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeGameStartPacket(int roomID, long time)
{
	flatbuffers::FlatBufferBuilder Builder;
	
	Builder.Finish(PacketTable::GameTable::CreateGameStart(Builder, roomID, time));
	return MakeBuffer(ePacketType::S2C_GAME_START, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeHeartBeatPacket()
{
	flatbuffers::FlatBufferBuilder Builder;
	
	long long currTime = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count();
	Builder.Finish(PacketTable::UtilitiesTable::CreateHeartBeat(Builder, currTime));
	return MakeBuffer(ePacketType::S2C_HEART_BEAT, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeBlockDropPacket(std::vector<std::pair<int, int>>& positions, std::vector<int>& blockTypes)
{
	flatbuffers::FlatBufferBuilder Builder;
	std::vector<flatbuffers::Offset<PacketTable::UtilitiesTable::Vec2i>> pos_vec;
	for (auto position : positions) {
		auto pos = PacketTable::UtilitiesTable::CreateVec2i(Builder, position.first, position.second);
		pos_vec.push_back(pos);
	}
	Builder.Finish(PacketTable::ObjectTable::CreateBlockDrop(Builder, Builder.CreateVector(pos_vec)
		, Builder.CreateVector(blockTypes)));
	return MakeBuffer(ePacketType::S2C_BLOCK_DROP, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeBombSpawnPacket(std::vector<Vector3f>& positions, std::vector<int>& bombIDs, int explosionInterval)
{
	flatbuffers::FlatBufferBuilder Builder;
	
	std::vector<flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f>> pos_vec;
	std::vector<int> delay_vec;

	for (Vector3f position : positions) {
		auto pos = PacketTable::UtilitiesTable::CreateVec3f(Builder, position.x, position.y, position.z);
		pos_vec.push_back(pos);
		delay_vec.push_back(explosionInterval);
	}
	Builder.Finish(PacketTable::ObjectTable::CreateBombSpawn(Builder, Builder.CreateVector(pos_vec),
		Builder.CreateVector(bombIDs)
		, Builder.CreateVector(delay_vec)));
	return MakeBuffer(ePacketType::S2C_BOMB_SPAWN, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeBombDropPacket(Vector3f position, int roomID, int bombID)
{
	flatbuffers::FlatBufferBuilder Builder;

	auto pos = PacketTable::UtilitiesTable::CreateVec3f(Builder, position.x, position.y, position.z);
	Builder.Finish(PacketTable::PlayerTable::CreatePlayerDropBomb(Builder, 0, pos, bombID));
	return MakeBuffer(ePacketType::S2C_PLAYER_DROP_BOMB, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeWeaponSpawnPacket(std::vector<Vector3f>& positions, std::vector<int>& weaponIDs, std::vector<int>& weaponTypes)
{
	flatbuffers::FlatBufferBuilder Builder;
	
	std::vector<flatbuffers::Offset<PacketTable::UtilitiesTable::Vec3f>> pos_vec;

	for (Vector3f position : positions) {
		auto pos = PacketTable::UtilitiesTable::CreateVec3f(Builder, position.x, position.y, position.z);
		pos_vec.push_back(pos);
	}
	Builder.Finish(PacketTable::ObjectTable::CreateWeaponSpawn(Builder, Builder.CreateVector(pos_vec),
		Builder.CreateVector(weaponIDs)
		, Builder.CreateVector(weaponTypes)));
	return MakeBuffer(ePacketType::S2C_WEAPON_SPAWN, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeWeaponDropPacket(Vector3f position, int weaponid)
{
	flatbuffers::FlatBufferBuilder Builder;
	
	auto pos = PacketTable::UtilitiesTable::CreateVec3f(Builder, position.x, position.y, position.z);
	Builder.Finish(PacketTable::ObjectTable::CreateWeaponPosition(Builder, pos, weaponid));
	return MakeBuffer(ePacketType::S2C_PLAYER_DROP_WEAPON, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeBombExplosionPacket(int bombID, Vector3f position)
{
	flatbuffers::FlatBufferBuilder Builder;
	
	auto pos = PacketTable::UtilitiesTable::CreateVec3f(Builder, position.x, position.y, position.x);
	Builder.Finish(PacketTable::ObjectTable::CreateBombExplosion(Builder, pos, bombID));
	return MakeBuffer(ePacketType::S2C_BOMB_EXPLOSION, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeGameStartPacket(int roomID, int startTime)
{
	flatbuffers::FlatBufferBuilder Builder;
	
	Builder.Finish(PacketTable::GameTable::CreateGameStart(Builder, roomID, startTime));
	return MakeBuffer(ePacketType::S2C_GAME_START, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeGameEndPacket(uint8_t winningTeams_flag)
{
	flatbuffers::FlatBufferBuilder Builder;
	
	Builder.Finish(PacketTable::GameTable::CreateGameEnd(Builder));
	return MakeBuffer(ePacketType::S2C_GAME_END, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeGameResultPacket(std::set<int>& winningTeams, std::unordered_map<int, sPlayerGameRecord>& records, std::unordered_map<int, class Player*>& players)
{
	flatbuffers::FlatBufferBuilder Builder;
	std::vector<flatbuffers::Offset<PacketTable::UtilitiesTable::PlayerGameRecord>> record_vec;
	std::vector<int> winningTeams_vector(winningTeams.begin(), winningTeams.end());


	for (auto& pair : records) {
		int id = pair.first;
		std::string playerName = "Unknown";
		if (players[id] != nullptr) {
			playerName = wstringToString(players[id]->GetNickName());
		}

		sPlayerGameRecord& record = pair.second;
		auto fRecord = PacketTable::UtilitiesTable::CreatePlayerGameRecord(Builder
			, id
			, Builder.CreateString(playerName)
			, record.team
			, record.gameRecord.KillCount
			, record.gameRecord.DeathCount
			, record.gameRecord.Bomb_Count
			, record.rewards[0].value
			, record.gameRecord.Point
			, record.is_mvp);

		record_vec.push_back(fRecord);
	}

	Builder.Finish(PacketTable::GameTable::CreateGameResult(Builder
		, Builder.CreateVector(winningTeams_vector)
		, Builder.CreateVector(record_vec)));

	return MakeBuffer(ePacketType::S2C_GAME_RESULT, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeGameHostChangePacket(int inGameID, int roomID)
{
	flatbuffers::FlatBufferBuilder Builder;
	
	Builder.Finish(PacketTable::GameTable::CreateGameHostChange(Builder, inGameID, roomID));
	return MakeBuffer(ePacketType::S2C_GAME_HOST_CHANGE, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeLifeReducePacket(int team, int lifeCount)
{
	flatbuffers::FlatBufferBuilder Builder;
	
	Builder.Finish(PacketTable::GameTable::CreateLifeReduce(Builder, team, lifeCount));
	return MakeBuffer(ePacketType::S2C_LIFE_REDUCE, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeRemainTimeSyncPacket(int roomID, TIMEPOINT gameStartTime, int gameEndTime)
{
	flatbuffers::FlatBufferBuilder Builder;
	
	TIMEPOINT currTime = std::chrono::system_clock::now();
	int elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currTime - gameStartTime).count();
	Builder.Finish(PacketTable::GameTable::CreateRemainTimeSync(Builder, gameEndTime - elapsedSeconds));
	return MakeBuffer(ePacketType::S2C_REMAIN_TIME_SYNC, Builder.GetBufferPointer(), Builder.GetSize());
}
