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
#include <windows.h>  
#undef max
#undef min

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


using namespace std;