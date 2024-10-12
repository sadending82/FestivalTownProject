#pragma once
#include <iostream>
#include <fstream>
#include <random>
#include <Ws2tcpip.h>
#include <winsock.h>
#include <WinSock2.h>
#include <atomic>
#include <memory>
#include <chrono>
#include <array>
#include <vector>
#include <thread>
#include <unordered_map>
#include <ctime>

#undef max
#undef min

#include "FlatBuffer/FlatBufferManager.h"
#include "../../Server/Server/Source/protocol.h"
