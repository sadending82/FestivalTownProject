#pragma once
#include "stdafx.h"
#include "utility.h"

class PacketMaker
{
public:
	PacketMaker() {};
	~PacketMaker() {};

	std::vector<uint8_t> MakeBuffer(const int type, const uint8_t* data, const int size);

	std::vector<uint8_t> MakeLoginRequestPacket(std::string& id, std::string& password);
};

