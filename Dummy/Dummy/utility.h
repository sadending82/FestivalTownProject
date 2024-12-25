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

struct Vector3f {
	float x;
	float y;
	float z;

	Vector3f() : x(0), y(0), z(0) {};
	Vector3f(float fx, float fy, float fz) : x(fx), y(fy), z(fz) {};

	bool operator == (const Vector3f& other) const {
		if (x == other.x && y == other.y && z == other.z) {
			return true;
		}
		return false;
	}

	bool operator < (const Vector3f& other) const {
		if (x != other.x) return x < other.x;
		if (y != other.y) return y < other.y;
		return z < other.z;
	}
};

enum ePlayerMoveState
{
    PS_WALK,
    PS_RUN,
    PS_JUMP,
    PS_MOVESTOP,
    PS_JUMPSTOP
};

inline constexpr int BUFSIZE = 4000;
inline constexpr int INVALID_ID = -1;