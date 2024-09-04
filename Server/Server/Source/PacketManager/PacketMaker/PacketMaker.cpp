#pragma once
#include "PacketMaker.h"

std::vector<uint8_t> PacketMaker::MakePlayerAdd(int inGameID)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	Builder.Finish(PacketTable::PlayerTable::CreatePlayerAdd(Builder, inGameID));
	return MakeBuffer(ePacketType::S2C_PLAYER_ADD, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakePlayerDeadPacket(int inGameID, int roomID, Vector3f position, Vector3f direction)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	auto pos = PacketTable::UtilitiesTable::CreateVec3f(Builder, position.x, position.y, position.x);
	auto dir = PacketTable::UtilitiesTable::CreateVec3f(Builder, direction.x, direction.y, direction.x);
	Builder.Finish(PacketTable::PlayerTable::CreatePlayerDead(Builder, inGameID, pos, dir));
	return MakeBuffer(ePacketType::S2C_PLAYER_DEAD, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakePlayerRespawnPacket(int inGameID, int roomID, int x, int y, int hp)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	auto pos = PacketTable::UtilitiesTable::CreateVec3f(Builder, x, y, 0);
	Builder.Finish(PacketTable::PlayerTable::CreatePlayerRespawn(Builder, inGameID, hp, pos));
	return MakeBuffer(ePacketType::S2C_PLAYER_RESPAWN, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakePlayerCalculatedDamagePacket(int targetID, int attackType, int hp, int damageAmount, Vector3f knockback_direction)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	auto dir = PacketTable::UtilitiesTable::CreateVec3f(Builder, knockback_direction.x, knockback_direction.y, knockback_direction.z);
	Builder.Finish(PacketTable::PlayerTable::CreatePlayerCalculatedDamage(Builder, targetID, attackType, hp, damageAmount, dir));
	return MakeBuffer(ePacketType::S2C_PLAYER_CALCULATED_DAMAGE, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeGameMatchingResponsePacket(int inGameID, int roomID, int team, int gameMode, int totalPlayerCount, bool isHost)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	Builder.Finish(PacketTable::LobbyTable::CreateGameMatchingResponse(Builder, inGameID, roomID, team, gameMode, isHost, totalPlayerCount));
	return MakeBuffer(ePacketType::S2C_MATCHING_RESPONSE, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeAllPlayerReadyPacket()
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	Builder.Finish(PacketTable::GameTable::CreateAllPlayerReady(Builder));
	return MakeBuffer(ePacketType::S2C_ALL_PLAYER_READY, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeGameStartPacket(int roomid, long time)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	Builder.Finish(PacketTable::GameTable::CreateGameStart(Builder, roomid, time));
	return MakeBuffer(ePacketType::S2C_GAME_START, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeHeartBeatPacket()
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	long long currTime = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count();
	Builder.Finish(PacketTable::UtilitiesTable::CreateHeartBeat(Builder, currTime));
	return MakeBuffer(ePacketType::S2C_HEART_BEAT, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeBlockDropPacket(int x, int y, int type)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	auto pos = PacketTable::UtilitiesTable::CreateVec2i(Builder, x, y);
	Builder.Finish(PacketTable::ObjectTable::CreateBlockDrop(Builder, pos, type));
	return MakeBuffer(ePacketType::S2C_BLOCK_DROP, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeBombSpawnPacket(Vector3f Positon, int bombid)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	auto pos = PacketTable::UtilitiesTable::CreateVec3f(Builder, Positon.x, Positon.y, Positon.z);
	Builder.Finish(PacketTable::ObjectTable::CreateBombSpawn(Builder, pos, bombid));
	return MakeBuffer(ePacketType::S2C_BOMB_SPAWN, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeWeaponSpawnPacket(Vector3f Positon, int weaponid)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	auto pos = PacketTable::UtilitiesTable::CreateVec3f(Builder, Positon.x, Positon.y, Positon.z);
	Builder.Finish(PacketTable::ObjectTable::CreateWeaponSpawn(Builder, pos, weaponid));
	return MakeBuffer(ePacketType::S2C_WEAPON_SPAWN, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeBombExplosionPacket(int bombID, Vector3f position)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	auto pos = PacketTable::UtilitiesTable::CreateVec3f(Builder, position.x, position.y, position.x);
	Builder.Finish(PacketTable::ObjectTable::CreateBombExplosion(Builder, pos, bombID));
	return MakeBuffer(ePacketType::S2C_BOMB_EXPLOSION, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeGameStartPacket(int roomID, int startTime)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	Builder.Finish(PacketTable::GameTable::CreateGameStart(Builder, roomID, startTime));
	return MakeBuffer(ePacketType::S2C_GAME_START, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeGameEndPacket(uint8_t winningTeams_flag)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	Builder.Finish(PacketTable::GameTable::CreateGameEnd(Builder, winningTeams_flag));
	return MakeBuffer(ePacketType::S2C_GAME_END, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeGameHostChangePacket(int inGameID, int roomID)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	Builder.Finish(PacketTable::GameTable::CreateGameHostChange(Builder, inGameID, roomID));
	return MakeBuffer(ePacketType::S2C_GAME_HOST_CHANGE, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeLifeReducePacket(int team, int lifeCount)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	Builder.Finish(PacketTable::GameTable::CreateLifeReduce(Builder, team, lifeCount));
	return MakeBuffer(ePacketType::S2C_LIFE_REDUCE, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeRemainTimeSyncPacket(int roomID, TIMEPOINT gameStartTime, int gameEndTime)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	TIMEPOINT currTime = std::chrono::system_clock::now();
	int elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currTime - gameStartTime).count();
	Builder.Finish(PacketTable::GameTable::CreateRemainTimeSync(Builder, gameEndTime - elapsedSeconds));
	return MakeBuffer(ePacketType::S2C_REMAIN_TIME_SYNC, Builder.GetBufferPointer(), Builder.GetSize());
}
