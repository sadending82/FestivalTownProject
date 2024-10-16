#define _WINSOCK_DEPRECATED_NO_WARNINGS
#undef max
#undef min
#include "stdafx.h"

#include "DummyClient.h"
#include "PacketMaker.h"
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib,"MSWSock.lib")
HANDLE g_hiocp;

#define PORTNUM 45872
#define IPADDRESS "127.0.0.1"
//"39.120.204.67"

inline constexpr int MAX_TEST = 5;
inline constexpr int MAX_CLIENTS = MAX_TEST * 2;

std::array<int, MAX_CLIENTS> client_map;
std::array<DummyClient, MAX_CLIENTS> g_clients;
std::atomic_int num_connections;
std::atomic_int client_to_close;
std::atomic_int active_clients;

int global_delay;

std::vector<std::thread*> workerThreads;
std::thread testThreads;

std::chrono::high_resolution_clock::time_point last_connect_time;

PacketMaker pm;

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_real_distribution<double> urd(-179.9, 179.9);

void TestThread();
void WorkerThread();
void ProcessPacket(unsigned char* data, const int ci);

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
	std::wcout << L"����" << lpMsgBuf << std::endl;

	LocalFree(lpMsgBuf);
}

void DisconnectClient(int ci)
{
	bool status = true;
	if (true == std::atomic_compare_exchange_strong(&g_clients[ci].connected, &status, false)) {
		closesocket(g_clients[ci].clientSocket);
		active_clients--;
	}

	std::cout << "Client [" << ci << "] Disconnected!\n";
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
		workerThreads.push_back(new std::thread{WorkerThread});

	testThreads = std::thread{ TestThread };
	testThreads.join();
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
			error_display("107 GQCS : ", WSAGetLastError());
			DisconnectClient(client_id);
			if (eOpType::OP_SEND == over->eventType) delete over;
		}

		if (0 == io_size) {
			//���� ����
			DisconnectClient(client_id);
			continue;
		}

		if (eOpType::OP_RECV == over->eventType) {
			unsigned char* packet_ptr = over->iocpBuf;
			uint16_t BufSize = 0;
			std::memcpy(&BufSize, packet_ptr, sizeof(uint16_t));
			unsigned char* buf = g_clients[ci].recvOver.iocpBuf;
			const int headerSize = sizeof(HEADER);
			int required_data = io_size + g_clients[ci].prevPacketData;
			int packet_size = BufSize + headerSize;
			while (required_data >= packet_size) {
				if (required_data >= BUFSIZE) break;
				if (packet_size <= 0) break;
				ProcessPacket(packet_ptr, ci);
				required_data -= packet_size;
				packet_ptr += packet_size;

				uint16_t nextBufSize = 0;
				std::memcpy(&nextBufSize, packet_ptr, sizeof(uint16_t));
				packet_size = nextBufSize + headerSize;
			}
			packet_size = 0;
			g_clients[ci].prevPacketData = 0;
			if (0 != required_data)
				memcpy(over->iocpBuf, packet_ptr, required_data);

			DWORD recv_flag = 0;
			int ret = WSARecv(g_clients[ci].clientSocket, &g_clients[ci].recvOver.wsabuf, 1, NULL, &recv_flag, &g_clients[ci].recvOver.over, NULL);
			if (SOCKET_ERROR == ret) {
				int err_no = WSAGetLastError();
				if (err_no != WSA_IO_PENDING)
				{
					error_display("145 RECV ERROR", err_no);
					DisconnectClient(client_id);
				}
			}
		}
		else if (eOpType::OP_SEND == over->eventType) {
			if (io_size != over->wsabuf.len) {
				std::cout << "152 Send Incomplete Error!\n";
				DisconnectClient(client_id);
			}
			delete over;
		}
		else {
			std::cout << "Unknown GQCS event! \n";
			while (true);
		}
	}
}

constexpr int DELAY_LIMIT = 100;
constexpr int DELAY_LIMIT2 = 150;
constexpr int ACCEPT_DELEY = 50;

void Adjust_Number_Of_Client()
{
	static int delay_multiplier = 1;
	static int max_limit = MAXINT;
	static bool increasing = true;

	if (active_clients >= MAX_TEST) return;
	if (num_connections >= MAX_CLIENTS) return;

	auto duration = std::chrono::high_resolution_clock::now() - last_connect_time;
	if (ACCEPT_DELEY * delay_multiplier > std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) return;

	int t_delay = global_delay;
	if (DELAY_LIMIT2 < t_delay) {
		if (true == increasing) {
			max_limit = active_clients;
			increasing = false;
		}
		if (100 > active_clients) return;
		if (ACCEPT_DELEY * 10 > std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) return;
		last_connect_time = std::chrono::high_resolution_clock::now();
		DisconnectClient(client_to_close);
		client_to_close++;
		return;
	}
	else {
		if (DELAY_LIMIT < t_delay) {
			delay_multiplier = 10;
			return;
		}
	}

	if (max_limit - (max_limit / 20) < active_clients) return;

	increasing = true;
	last_connect_time = std::chrono::high_resolution_clock::now();
	g_clients[num_connections].clientSocket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(PORTNUM);
	ServerAddr.sin_addr.S_un.S_addr = inet_addr(IPADDRESS);

	int Result = WSAConnect(g_clients[num_connections].clientSocket, (sockaddr*)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);
	if (0 != Result) {
		std::cout << "WSAConnect Error\n";
	}

	g_clients[num_connections].currPacketSize = 0;
	g_clients[num_connections].prevPacketData = 0;
	ZeroMemory(&g_clients[num_connections].recvOver, sizeof(g_clients[num_connections].recvOver));
	g_clients[num_connections].recvOver.eventType = eOpType::OP_RECV;
	g_clients[num_connections].recvOver.wsabuf.buf = reinterpret_cast<CHAR*>(g_clients[num_connections].recvOver.iocpBuf);
	g_clients[num_connections].recvOver.wsabuf.len = sizeof(g_clients[num_connections].recvOver.iocpBuf);
	
	DWORD recv_flag = 0;
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_clients[num_connections].clientSocket), g_hiocp, num_connections, 0);

	std::string idstr;
	std::string pwstr;
	idstr.append("test");
	char numstr[20];
	int index = num_connections;
	sprintf_s(numstr, "%d", index);
	idstr.append(numstr);
	pwstr.append("1234");
	auto packet = pm.MakeLoginRequestPacket(idstr, pwstr);
	g_clients[num_connections].DoSend(packet.data(), packet.size());

	int ret = WSARecv(g_clients[num_connections].clientSocket, &g_clients[num_connections].recvOver.wsabuf, 1,
		NULL, &recv_flag, &g_clients[num_connections].recvOver.over, NULL);
	if (SOCKET_ERROR == ret)
	{
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING)
		{
			std::cout << "245 RECV ERROR\n";
			goto fail_to_connect;
		}
	}
	num_connections++;
fail_to_connect:
	return;
}

using namespace std::chrono;

void TestThread()
{
	while (true) {
		Adjust_Number_Of_Client();

		for (int i = 0; i < num_connections; ++i)
		{
			if (false == g_clients[i].connected) continue;
			if (false == g_clients[i].isInGame) continue;
			if (g_clients[i].lastPacketSend + 32ms > high_resolution_clock::now()) continue;
			g_clients[i].lastPacketSend = high_resolution_clock::now();
			g_clients[i].direction.z += 0.001;
			auto pack = pm.MakePlayerMovePacket(g_clients[i].position, g_clients[i].direction, g_clients[i].ingameId, ePlayerMoveState::PS_RUN);
			auto pack2 = pm.MakePlayerPosSyncPacket(g_clients[i].ingameId, g_clients[i].position, g_clients[i].direction, 100);
			g_clients[i].DoSend(pack.data(), pack.size());
			g_clients[i].DoSend(pack2.data(), pack2.size());
		}
	}
}

void ProcessPacket(unsigned char* data, const int ci)
{
	
	HEADER* header = reinterpret_cast<HEADER*>(data);
	int type = header->type;
	int flatBufSize = header->flatBufferSize;
	int headerSize = sizeof(HEADER);

	switch ((ePacketType)type)
	{
	case ePacketType::S2C_LOGIN_RESPONSE:
	{
		g_clients[ci].connected = true;
		std::cout << ci << "�� �α����� �õ��߽��ϴ�." << std::endl;
		std::cout << ci << "�� ��Ī�� �õ��ؿ�!" << std::endl;
		auto packet = pm.MakeMatchingRequestPacket(ci);
		g_clients[ci].DoSend(packet.data(), packet.size());
	}	
		break;
	case ePacketType::S2C_SIGNUP_RESPONSE:
	{
		
	}
		break;
	case ePacketType::S2C_GAME_START:
	{
		std::cout << ci << "�÷��̾ ���� ���۰� ���ÿ� ���� �����մϴ�!" << std::endl;
		g_clients[ci].isInGame = true;
	}
		break;
	case ePacketType::S2C_PLAYER_ADD:
	{

		uint8_t* d = data + headerSize;
		size_t s = header->flatBufferSize;
		flatbuffers::Verifier verifier(d, s);
		if (verifier.VerifyBuffer<PacketTable::PlayerTable::PlayerAdd>(nullptr)) {
			const PacketTable::PlayerTable::PlayerAdd* read = flatbuffers::GetRoot<PacketTable::PlayerTable::PlayerAdd>(d);

			for (auto pl : *(read->players()))
			{
				if (pl->id() == g_clients[ci].ingameId) {
					g_clients[ci].position.x = pl->pos()->x();
					g_clients[ci].position.y = pl->pos()->y() + 0.5;
					g_clients[ci].position.z = pl->pos()->z();
				}
			}
		}
	}
		break;

	case ePacketType::S2C_MATCHING_RESPONSE:
	{
		std::cout << ci << "�÷��̾ ��Ī�� �� ���� Ȯ���߾��!" << std::endl;

		uint8_t* d = data + headerSize;
		size_t s = header->flatBufferSize;
		flatbuffers::Verifier verifier(d, s);
		if (verifier.VerifyBuffer<PacketTable::LobbyTable::GameMatchingResponse>(nullptr)) {
			const PacketTable::LobbyTable::GameMatchingResponse* read = flatbuffers::GetRoot<PacketTable::LobbyTable::GameMatchingResponse>(d);

			g_clients[ci].roomId = read->roomid();
			g_clients[ci].ingameId = read->ingameid();
		}

		auto packet = pm.MakeGameReadyPacket(g_clients[ci].roomId);
		g_clients[ci].DoSend(packet.data(), packet.size());
	}
		break;
	case ePacketType::S2C_GAME_END:
	{
		std::cout << ci << "�� ������ �����ٰ� �˸��ϴ�." << std::endl;
		g_clients[ci].isInGame = false;
		g_clients[ci].direction.z = 0;
	}
		break;
	case ePacketType::S2C_GAME_RESULT:
	{
		std::cout << ci << "�� ����� ���� �޾Ҿ��." << std::endl;
		std::cout << ci << "�� �ٷ� �ٽ� ��Ī�� ��û�ؿ�" << std::endl;
		auto packet = pm.MakeMatchingRequestPacket(ci);
		g_clients[ci].DoSend(packet.data(), packet.size());
	}
		break;
	case ePacketType::S2C_HEART_BEAT:
	{
		auto packet = pm.MakeHeartBeatPacket();
		g_clients[ci].DoSend(packet.data(), packet.size());
	}
		break;
	default:
		break;
	}
}

