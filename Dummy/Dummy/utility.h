#pragma once
#include <iostream>
#include <chrono>
#include "stdafx.h"
#include "../../Server/Server/Source/protocol.h"

#define TIMEPOINT std::chrono::system_clock::time_point

struct HEADER {
public:
	uint16_t	flatBufferSize;
	uint16_t	type;
};

inline constexpr int BUFSIZE = 1000;
inline constexpr int INVALID_ID = -1;