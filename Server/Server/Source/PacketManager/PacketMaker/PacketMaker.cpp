#pragma once
#include "PacketMaker.h"

std::vector<uint8_t> PacketMaker::MakePlayerAdd(int inGameID)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	Builder.Finish(PacketTable::PlayerTable::CreatePlayerAdd(Builder, inGameID));
	return MakeBuffer(ePacketType::S2C_PLAYERADD, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakePlayerGameInfo(int inGameID, int roomID)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	Builder.Finish(PacketTable::PlayerTable::CreatePlayerGameInfo(Builder, inGameID, roomID));
	return MakeBuffer(ePacketType::S2C_PLAYERGAMEINFO, Builder.GetBufferPointer(), Builder.GetSize());
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

std::vector<uint8_t> PacketMaker::MakeObjectDropPacket(int x, int y, int type)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	auto pos = PacketTable::ObjectTable::CreateVec2i(Builder, x, y);
	Builder.Finish(PacketTable::ObjectTable::CreateObjectDrop(Builder, pos, type));
	return MakeBuffer(ePacketType::S2C_OBJECTDROP, Builder.GetBufferPointer(), Builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeBombSpawnPacket(int x, int y)
{
	flatbuffers::FlatBufferBuilder Builder;
	Builder.Clear();
	auto pos = PacketTable::ObjectTable::CreateVec2i(Builder, x, y);
	Builder.Finish(PacketTable::ObjectTable::CreateBombSpawn(Builder, pos));
	return MakeBuffer(ePacketType::S2C_BOMBSPAWN, Builder.GetBufferPointer(), Builder.GetSize());
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