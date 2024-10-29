#include "stdafx.h"
#include "PacketMaker.h"

std::vector<uint8_t> PacketMaker::MakeBuffer(const int type, const uint8_t* data, const int size)
{
    HEADER h;
    h.flatBufferSize = size;
    h.type = type;

    std::vector<uint8_t> buf(sizeof(HEADER) + size);
    memcpy(buf.data(), &h, sizeof(HEADER));
    memcpy(buf.data() + sizeof(HEADER), data, size);

    return buf;
}

std::vector<uint8_t> PacketMaker::MakeLoginRequestPacket(std::string& id, std::string& password)
{
    flatbuffers::FlatBufferBuilder builder;

    builder.Finish(PacketTable::LoginTable::CreateLoginRequest(builder, builder.CreateString(id), builder.CreateString(password)));
    return MakeBuffer(ePacketType::C2S_LOGIN_REQUEST, builder.GetBufferPointer(), builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakePlayerMovePacket(Vector3f pos, Vector3f dir, int id, ePlayerMoveState state)
{
    flatbuffers::FlatBufferBuilder builder;

    auto bpos = PacketTable::UtilitiesTable::CreateVec3f(builder, pos.x, pos.y, pos.z);
    auto bdir = PacketTable::UtilitiesTable::CreateVec3f(builder, dir.x, dir.y, dir.z);
    builder.Finish(PacketTable::PlayerTable::CreatePlayerMove(builder, id, bpos, bdir, state));
    return MakeBuffer(ePacketType::C2S_PLAYER_MOVE, builder.GetBufferPointer(), builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeHeartBeatPacket()
{
    flatbuffers::FlatBufferBuilder builder;
    long long currTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    builder.Finish(PacketTable::UtilitiesTable::CreateHeartBeat(builder, currTime));
    return MakeBuffer(ePacketType::C2S_HEART_BEAT, builder.GetBufferPointer(), builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeMatchingRequestPacket(int id, eMatchingType type)
{
    flatbuffers::FlatBufferBuilder builder;
    builder.Finish(PacketTable::LobbyTable::CreateGameMatchingRequest(builder, id, type));
    return MakeBuffer(ePacketType::C2S_MATCHING_REQUEST, builder.GetBufferPointer(), builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeGameReadyPacket(int roomid)
{
    flatbuffers::FlatBufferBuilder builder;
    builder.Finish(PacketTable::GameTable::CreateGameReady(builder, roomid));
    return MakeBuffer(ePacketType::C2S_GAME_READY, builder.GetBufferPointer(), builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakePlayerPosSyncPacket(int id, Vector3f pos, Vector3f dir, int stamina)
{
    flatbuffers::FlatBufferBuilder builder;
    auto bpos = PacketTable::UtilitiesTable::CreateVec3f(builder, pos.x, pos.y, pos.z);
    auto bdir = PacketTable::UtilitiesTable::CreateVec3f(builder, dir.x, dir.y, dir.z);
    builder.Finish(PacketTable::PlayerTable::CreatePlayerSync(builder, id, bpos, bdir, stamina));
    return MakeBuffer(ePacketType::C2S_PLAYER_SYNC, builder.GetBufferPointer(), builder.GetSize());
}
