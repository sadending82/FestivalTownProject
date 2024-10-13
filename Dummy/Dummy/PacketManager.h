#pragma once
#include "stdafx.h"
#include "PacketProcessor.h"


class PacketManager
{
public:
	PacketManager() {};
	~PacketManager() {};

	void Init();
	void ProcessPacket(unsigned char* data, const int key);
	
private:
	std::unordered_map<int, std::unique_ptr<PacketProcessor>> PacketProcessorMap;

};

