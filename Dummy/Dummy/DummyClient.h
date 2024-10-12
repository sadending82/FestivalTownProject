#pragma once
#undef max
#undef min
#include "stdafx.h"
#include "OverlappedEx.h"


#include "utility.h"

class DummyClient
{
public:
	DummyClient();
	~DummyClient();

	void error_display(const char* msg, int err_no)
	{
		WCHAR* lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, err_no,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf, 0, NULL);
		std::cout << msg;
		std::wcout << L"¿¡·¯" << lpMsgBuf << std::endl;

		LocalFree(lpMsgBuf);
	}

	int id;
	float x, y, z;
	std::atomic_bool connected;
	SOCKET clientSocket;
	OverlappedEx recvOver;
	unsigned char packetBuf[BUFSIZE];
	int prevPacketData;
	int currPacketSize;
	std::chrono::high_resolution_clock::time_point lastPacketSend;
};

