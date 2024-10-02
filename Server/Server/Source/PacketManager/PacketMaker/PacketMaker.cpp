#pragma once
#include "PacketMaker.h"
#include "../../Object/Player.h"

std::vector<uint8_t> PacketMaker::MakePlayerAddPacket(std::array<class Player*, MAXPLAYER>& players)
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
		auto pInfo = PacketTable::PlayerTable::CreatePlayerInfo(Builder
			, player->GetInGameID()
			, pos
			, dir
			, player->GetTeam()
			, player->GetChacracterType()
			, Builder.CreateString(player->GetName())
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

std::vector<uint8_t> PacketMaker::MakePlayerGroggyPacket(int id)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Finish(PacketTable::PlayerTable::CreatePlayerGroggy(Builder, id));
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

std::vector<uint8_t> PacketMaker::MakeGameMatchingResponsePacket(int inGameID, int roomID, int team, int gameMode, int gameTime, int totalPlayerCount, bool isHost)
{
	flatbuffers::FlatBufferBuilder Builder;
	
	Builder.Finish(PacketTable::LobbyTable::CreateGameMatchingResponse(Builder, inGameID, roomID, team, gameMode, gameTime, isHost, totalPlayerCount));
	return MakeBuffer(ePacketType::S2C_MATCHING_RESPONSE, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeAllPlayerReadyPacket()
{
	flatbuffers::FlatBufferBuilder Builder;
	
	Builder.Finish(PacketTable::GameTable::CreateAllPlayerReady(Builder));
	return MakeBuffer(ePacketType::S2C_ALL_PLAYER_READY, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeGameStartPacket(int roomid, long time)
{
	flatbuffers::FlatBufferBuilder Builder;
	
	Builder.Finish(PacketTable::GameTable::CreateGameStart(Builder, roomid, time));
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

std::vector<uint8_t> PacketMaker::MakeGameResultPacket(std::set<int>& winningTeams, std::unordered_map<int, sPlayerGameRecord>& records, std::array<class Player*, MAXPLAYER>& players)
{
	flatbuffers::FlatBufferBuilder Builder;
	std::vector<flatbuffers::Offset<PacketTable::UtilitiesTable::PlayerGameRecord>> record_vec;
	std::vector<int> winningTeams_vector(winningTeams.begin(), winningTeams.end());


	for (auto& pair : records) {
		int id = pair.first;
		if (players[id] == nullptr) {
			continue;
		}
		sPlayerGameRecord record = pair.second;
		auto fRecord = PacketTable::UtilitiesTable::CreatePlayerGameRecord(Builder
			, id
			, Builder.CreateString(players[id]->GetName())
			, record.team
			, record.kill_count
			, record.death_count
			, record.bomb_insert_count
			, record.earn_gold
			, record.point
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
