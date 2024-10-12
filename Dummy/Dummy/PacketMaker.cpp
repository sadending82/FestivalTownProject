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
