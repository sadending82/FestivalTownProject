#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <unordered_set>
#include <queue>
#include <chrono>
#include <random>
#include <string>
#include <locale.h>
#include <cctype>
#include <memory>

#define _WINSOCKAPI_
#include <windows.h>  
#include <winsock2.h>
#include <mswsock.h>
#undef max
#undef min

#pragma comment (lib,"WS2_32.lib")
#pragma comment (lib,"MSWSock.lib")

constexpr int STARTKEY = 1;
constexpr int INVALIDKEY = -1;
constexpr int BUFSIZE = 200;
constexpr int MAXPLAYER = 100;

#ifdef _DEBUG
#define DEBUGMSGNOPARAM(args) (printf("file: %s, line:%d ",__FILE__,__LINE__), printf(args))
#define DEBUGMSGONEPARAM(args,k) (printf("file: %s, line:%d ",__FILE__,__LINE__), printf(args,k))
#else
#define DEBUGMSGNOPARAM(...)
#define DEBUGMSGONEPARAM(...)
#endif

#pragma pack (push, 1)

struct HEADER {
public:
	uint16_t	size;
	uint16_t	type;
};
#pragma pack(pop)

std::vector<uint8_t> MakeBuffer(char type, const uint8_t* data, const int size);