#pragma once
#include "Packet/PacketPlayerMove.h"
#include "Packet/PacketPlayerStop.h"
#include "Event/EventTest.h"

enum PACKETTYPE {
	S2C_ADDPLAYER,
	S2C_PLAYERMOVE,
	S2C_PLAYERSTOP,


	C2S_PLAYERMOVE,
	C2S_PLAYERSTOP,
};