#pragma once
#include "PacketMaker.h"

std::vector<uint8_t> PacketMaker::MakePlayerAdd(int inGameID)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	Builder.Finish(PacketTable::PlayerTable::CreatePlayerAdd(Builder, inGameID));
	return MakeBuffer(ePacketType::S2C_PLAYERADD, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeGameInfo(int inGameID, int roomID, int team, int gameMode, bool isHost)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	Builder.Finish(PacketTable::GameTable::CreateGameInfo(Builder, inGameID, roomID, team, gameMode, isHost));
	return MakeBuffer(ePacketType::S2C_GAMEINFO, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeHeartBeatPacket()
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	long long currTime = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count();
	Builder.Finish(PacketTable::UtilitiesTable::CreateHeartBeat(Builder, currTime));
	return MakeBuffer(ePacketType::S2C_HEARTBEAT, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeBlockDropPacket(int x, int y, int type)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	auto pos = PacketTable::ObjectTable::CreateVec2i(Builder, x, y);
	Builder.Finish(PacketTable::ObjectTable::CreateBlockDrop(Builder, pos, type));
	return MakeBuffer(ePacketType::S2C_BLOCKDROP, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeBombSpawnPacket(int x, int y, int bombid)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	auto pos = PacketTable::ObjectTable::CreateVec2i(Builder, x, y);
	Builder.Finish(PacketTable::ObjectTable::CreateBombSpawn(Builder, pos, bombid));
	return MakeBuffer(ePacketType::S2C_BOMBSPAWN, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeBombExplosionPacket(int bombID, Vector3f position)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	auto pos = PacketTable::ObjectTable::CreateVec3f(Builder, position.x, position.y, position.x);
	Builder.Finish(PacketTable::ObjectTable::CreateBombExplosion(Builder, pos, bombID));
	return MakeBuffer(ePacketType::S2C_BOMBEXPLOSION, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeGameStartPacket(int roomID, int startTime)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	Builder.Finish(PacketTable::GameTable::CreateGameStart(Builder, roomID, startTime));
	return MakeBuffer(ePacketType::S2C_GAMESTART, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeGameEndPacket(int winningTeam)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	Builder.Finish(PacketTable::GameTable::CreateGameEnd(Builder, winningTeam));
	return MakeBuffer(ePacketType::S2C_GAMEEND, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeLifeReducePacket(int team, int lifeCount)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	Builder.Finish(PacketTable::GameTable::CreateLifeReduce(Builder, team, lifeCount));
	return MakeBuffer(ePacketType::S2C_LIFEREDUCE, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeRemainTimeSyncPacket(int roomID, TIMEPOINT gameStartTime, int gameEndTime)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	TIMEPOINT currTime = std::chrono::system_clock::now();
	int elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currTime - gameStartTime).count();
	Builder.Finish(PacketTable::GameTable::CreateRemainTimeSync(Builder, gameEndTime - elapsedSeconds));
	return MakeBuffer(ePacketType::S2C_REMAINTIMESYNC, Builder.GetBufferPointer(), Builder.GetSize());
}