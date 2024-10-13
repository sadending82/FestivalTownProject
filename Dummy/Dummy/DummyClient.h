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

	void DoSend(void* packet, const size_t size)
	{
		OverlappedEx* sdata = new OverlappedEx{ reinterpret_cast<unsigned char*>(packet), (int)size };
		int ret = WSASend(clientSocket, &sdata->wsabuf, 1, 0, 0, &sdata->over, 0);
		if (0 != ret) {
			int err_no = WSAGetLastError();
			if (WSA_IO_PENDING != err_no)
				std::cout << "Error in SendPacket : " << err_no << std::endl;
		}
	}

	int id;
	Vector3f position;
	Vector3f direction;
	std::atomic_bool connected;
	SOCKET clientSocket;
	OverlappedEx recvOver;
	unsigned char packetBuf[BUFSIZE];
	int prevPacketData;
	int currPacketSize;
	std::chrono::high_resolution_clock::time_point lastPacketSend;
	bool isInGame;
	int roomId;
	int ingameId;
	
};

