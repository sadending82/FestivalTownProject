#undef max
#undef min
#include "stdafx.h"

#include "DummyClient.h"

#pragma comment (lib, "ws2_32.lib")

HANDLE g_hiocp;

inline constexpr int MAX_CLIENTS = 500;

std::array<int, MAX_CLIENTS> client_map;
std::array<DummyClient, MAX_CLIENTS> g_clients;
std::atomic_int num_connections;
std::atomic_int client_to_close;
std::atomic_int active_clients;

int global_delay;

std::vector<std::thread*> workerThreads;
std::thread testThreads;

std::chrono::high_resolution_clock::time_point last_connect_time;

void DisconnectClient(int ci)
{
	bool status = true;
	if (true == std::atomic_compare_exchange_strong(&g_clients[ci].connected, &status, false)) {
		closesocket(g_clients[ci].clientSocket);
		active_clients--;
	}
}

int main()
{
	for (auto& cl : g_clients)
	{
		cl.connected = false;
		cl.id = INVALID_ID;
	}

	for (auto& cl : client_map) cl = -1;
	num_connections = 0;
	last_connect_time = std::chrono::high_resolution_clock::now();

	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	g_hiocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);

	for (int i = 0; i < 6; ++i)
		workerThreads.push_back(new std::thread{});
}

void WorkerThread()
{
	while (true)
	{
		DWORD io_size;
		unsigned long long ci;
		OverlappedEx* over;
		BOOL ret = GetQueuedCompletionStatus(g_hiocp, &io_size, &ci, reinterpret_cast<LPWSAOVERLAPPED*>(&over), INFINITE);

		int client_id = static_cast<int>(ci);
		if (FALSE == ret) {
			int err_no = WSAGetLastError();
			if (64 == err_no) DisconnectClient(client_id);
			else {
				DisconnectClient(client_id);
			}
			if (eOpType::OP_SEND == over->eventType) delete over;
		}

		if (0 == io_size) {
			DisconnectClient(client_id);
			continue;
		}

		if (eOpType::OP_RECV == over->eventType) {
			unsigned char* buf = g_clients[ci].recvOver.iocpBuf;
			unsigned psize = g_clients[ci].currPacketSize;
			unsigned pr_size = g_clients[ci].prevPacketData;
			while (io_size > 0) {
				if (0 == psize) psize = buf[0];
				if (io_size + pr_size >= psize) {
					unsigned char packet[BUFSIZE];
					memcpy(packet, g_clients[ci].packetBuf, pr_size);
					memcpy(packet + pr_size, buf, psize - pr_size);
					//processpacket
					io_size -= psize - pr_size;
					buf += psize - pr_size;
					psize = 0; pr_size = 0;
				}
				else {
					memcpy(g_clients[ci].packetBuf + pr_size, buf, io_size);
					pr_size += io_size;
					io_size = 0;
				}
			}
			g_clients[ci].currPacketSize = psize;
			g_clients[ci].prevPacketData = pr_size;
			DWORD recv_flag = 0;
			int ret = WSARecv(g_clients[ci].clientSocket, &g_clients[ci].recvOver.wsabuf, 1, NULL, &recv_flag, &g_clients[ci].recvOver.over, NULL);
			if (SOCKET_ERROR == ret) {
				int err_no = WSAGetLastError();
				if (err_no != WSA_IO_PENDING)
				{

					DisconnectClient(client_id);
				}
			}
		}
	}
}