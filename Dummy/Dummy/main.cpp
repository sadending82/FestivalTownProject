#define _WINSOCK_DEPRECATED_NO_WARNINGS
#undef max
#undef min
#include "stdafx.h"

#include "DummyClient.h"
#include "PacketMaker.h"
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib,"MSWSock.lib")
HANDLE g_hiocp;

//#define PORTNUM 45872
#define PORTNUM 30432
//#define IPADDRESS "127.0.0.1"
#define IPADDRESS "203.239.231.148"

inline constexpr int MAX_TEST = 100;
inline constexpr int MAX_CLIENTS = 100;

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
	std::wcout << L"에러" << lpMsgBuf << std::endl;

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

	for (int i = 0; i < 12; ++i)
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
			//연결 끊김
			DisconnectClient(client_id);
			continue;
		}

		if (eOpType::OP_RECV == over->eventType) {

			const int headerSize = sizeof(HEADER);

			unsigned char* buf = g_clients[ci].recvOver.iocpBuf;		
			uint16_t psize = g_clients[ci].currPacketSize;
			unsigned prSize = g_clients[ci].prevPacketData;

			// 받은 데이터가 실제 패킷 크기보다 클 동안 패킷 재조립 실행
			while (io_size > 0)
			{
				if (0 == psize) {
					std::memcpy(&psize, buf, sizeof(uint16_t));
					psize += headerSize;
				}
				if (io_size + prSize >= psize) {
					//패킷 조립 가능
					unsigned char packet[BUFSIZE];
					memcpy(packet, g_clients[ci].packetBuf, prSize);
					memcpy(packet + prSize, buf, psize - prSize);
					ProcessPacket(packet, static_cast<int>(ci));
					io_size -= psize - prSize;
					buf += psize - prSize;
					psize = 0; prSize = 0;
				}
				else {
					memcpy(g_clients[ci].packetBuf + prSize, buf, io_size);
					prSize += io_size;
					io_size = 0;
				}
			}

			g_clients[ci].currPacketSize = psize;
			g_clients[ci].prevPacketData = prSize;

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
constexpr int ACCEPT_DELEY = 100;

void Adjust_Number_Of_Client()
{
	static int delay_multiplier = 1;
	static int max_limit = MAXINT;
	static bool increasing = true;

	if (active_clients >= MAX_TEST) {
		//std::cout << "끝\n";
		return;
	}
	if (num_connections >= MAX_CLIENTS) {
		//std::cout << "끝\n";
		return;
	}

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

	/*std::string idstr;
	std::string pwstr;
	idstr.append("test");
	char numstr[20];
	int index = num_connections;
	sprintf_s(numstr, "%d", index);
	idstr.append(numstr);
	pwstr.append("1234");
	auto packet = pm.MakeLoginRequestPacket(idstr, pwstr);
	g_clients[num_connections].DoSend(packet.data(), packet.size());*/

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> matchingType_distrib(0, 10);

	g_clients[num_connections].connected = true;
	/*std::cout << num_connections << " 접속 완료" << std::endl;
	std::cout << num_connections << " 매칭 요청" << std::endl;*/
	eMatchingType mtype;
	if (matchingType_distrib(gen) % 2 == 0) {
		mtype = eMatchingType::FITH_SOLO;
	}
	else {
		mtype = eMatchingType::FITH_TEAM;
	}
	auto packet = pm.MakeMatchingRequestPacket(num_connections, mtype);
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
			if (g_clients[i].lastSyncPacketSend + (16ms*6) <= high_resolution_clock::now())
			{
				g_clients[i].lastSyncPacketSend = high_resolution_clock::now();
				auto pack = pm.MakePlayerPosSyncPacket(g_clients[i].ingameId, g_clients[i].position, g_clients[i].direction, 100);
				g_clients[i].DoSend(pack.data(), pack.size());
			}
			if (g_clients[i].lastMovePacketSend + 1s <= high_resolution_clock::now())
			{
				g_clients[i].lastMovePacketSend = high_resolution_clock::now();
				g_clients[i].direction.y += 0.005;
				auto pack = pm.MakePlayerMovePacket(g_clients[i].position, g_clients[i].direction,
					g_clients[i].ingameId, g_clients[i].isMove ? ePlayerMoveState::PS_RUN : ePlayerMoveState::PS_MOVESTOP);
				g_clients[i].isMove = !g_clients[i].isMove;
				g_clients[i].DoSend(pack.data(), pack.size());
			}
		}
	}
}

void ProcessPacket(unsigned char* data, const int ci)
{
	
	HEADER* header = reinterpret_cast<HEADER*>(data);
	int type = header->type;
	int flatBufSize = header->flatBufferSize;
	int headerSize = sizeof(HEADER);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> matchingType_distrib(0, 1);

	switch ((ePacketType)type)
	{
	case ePacketType::S2C_LOGIN_RESPONSE:
	{
		
	}	
		break;
	case ePacketType::S2C_SIGNUP_RESPONSE:
	{
		
	}
		break;
	case ePacketType::S2C_GAME_START:
	{
		//std::cout << ci << "플레이어가 게임 시작과 동시에 댄스를 시작합니다!" << std::endl;
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
		//std::cout << ci << " 플레이어가 매칭완료" << std::endl;

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
		//std::cout << ci << "가 게임이 끝났다고 알립니다." << std::endl;
		g_clients[ci].isInGame = false;
		g_clients[ci].direction.z = 0;
	}
		break;
	case ePacketType::S2C_GAME_RESULT:
	{
		//std::cout << ci << "가 결과를 전송 받았어요." << std::endl;
		//std::cout << ci << "가 바로 다시 매칭을 신청해요" << std::endl;

		eMatchingType mtype;
		if (matchingType_distrib(gen) == 0) {
			mtype = eMatchingType::FITH_SOLO;
		}
		else {
			mtype = eMatchingType::FITH_TEAM;
		}

		auto packet = pm.MakeMatchingRequestPacket(ci, mtype);
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

