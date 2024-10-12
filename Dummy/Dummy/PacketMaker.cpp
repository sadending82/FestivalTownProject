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
    std::cout << "Make Hart Beat Packet" << std::endl;
    flatbuffers::FlatBufferBuilder builder;
    int64_t t = time(NULL);
    builder.Finish(PacketTable::UtilitiesTable::CreateHeartBeat(builder, t));
    return MakeBuffer(ePacketType::C2S_HEART_BEAT, builder.GetBufferPointer(), builder.GetSize());
}

std::vector<uint8_t> PacketMaker::MakeMatchingRequestPacket(int id)
{
    
    flatbuffers::FlatBufferBuilder builder;
    builder.Finish(PacketTable::LobbyTable::CreateGameMatchingRequest(builder, id));
    return MakeBuffer(ePacketType::C2S_MATCHING_REQUEST, builder.GetBufferPointer(), builder.GetSize());
}
