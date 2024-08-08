#pragma once
#include <random>
#include "Server.h"
#include "../Thread/WorkerThread/WorkerThread.h"
#include "../Thread/TimerThread/TimerThread.h"
#include "../TableManager/TableManager.h"
#include "../Event/Event.h"

int Server::SetSessionKey()
{
    for (int i = STARTKEY; i < MAXSESSION; ++i) {
        auto session = GetSessions()[i];
        if (session == nullptr) continue;
        if (eSessionState::ST_FREE == session->GetState()) {
            return i;
        }
    }
    DEBUGMSGNOPARAM("Set Key Error\n");
    return INVALIDKEY;
}

void Server::Disconnect(int key)
{
    Player* session = dynamic_cast<Player*>(GetSessions()[key]);
    if (session->GetState() == eSessionState::ST_FREE) {
        return;
    }

    // Delete Player In Room
    int roomID;
    if (roomID = session->GetRoomID() != INVALIDKEY) {
        mRooms[roomID]->DeletePlayer(session->GetInGameID());
    }

    closesocket(session->GetSocket());
    session->SetState(eSessionState::ST_FREE);

    DEBUGMSGONEPARAM("Disconnect: %d\n", key);
}

void Server::Init(class TableManager* pTableManager, class DB* pDB)
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    mHcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
    if (mHcp == NULL) return;

    mListenSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    CreateIoCompletionPort(reinterpret_cast<HANDLE>(mListenSocket), mHcp, 0, 0);
    SOCKADDR_IN serverAddr;
    memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVERPORT);
    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    ::bind(mListenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN));
    listen(mListenSocket, SOMAXCONN);

    ExOver accept_over;
    accept_over.mOpType = eOpType::OP_ACCEPT;
    memset(&accept_over.mOver, 0, sizeof(accept_over.mOver));
    SOCKET c_socket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    accept_over.mWsaBuf.buf = reinterpret_cast<char*>(c_socket);
    BOOL ret = AcceptEx(mListenSocket, c_socket, accept_over.mMessageBuf
        , 0, 32, 32, NULL, &accept_over.mOver);

    if (false == ret) {
        int err_num = WSAGetLastError();
        if (err_num != WSA_IO_PENDING)
        {
            DEBUGMSGONEPARAM("Accept Error: %d\n", err_num);
            return;
        }
    }
    DEBUGMSGNOPARAM("Accept Ready\n");



    for (int i = 0; i < MAXSESSION; ++i) {
        mSessions[i] = new Player();
    }
    for (int i = 0; i < MAXROOM; ++i) {
        mRooms[i] = new Room();
    }

    pDB->Init();

    // Thread Create
    mTimer = new Timer;
    mTimer->Init(mHcp);

    // 테스트를 위해 임시 방 추가
    mRooms[TESTROOM]->Init(TESTROOM);
    // 이벤트 테스트
    PushEventObjectDrop(mTimer, TESTROOM);

    SYSTEM_INFO si;
    GetSystemInfo(&si);
    for (int i = 0; i < (int)si.dwNumberOfProcessors; ++i) {
        WorkerThread* pWorkerThreadRef = new WorkerThread(this);
        mWorkerThreads.emplace_back(std::thread(&WorkerThread::RunWorker, pWorkerThreadRef));
    }
    mTimerThread = std::thread(&Timer::Main, mTimer);

    DEBUGMSGNOPARAM("Thread Ready\n");
}

void Server::ThreadJoin()
{
    for (auto& th : mWorkerThreads)
        th.join();
    mTimerThread.join();
}

void Server::SendAllPlayerInRoomBySessionID(void* packet, int size, int sessionID)
{
    int roomID = dynamic_cast<Player*>(GetSessions()[sessionID])->GetRoomID();
    for (Player* p : GetRooms()[roomID]->GetPlayerList()) {
        if (p == nullptr) continue;
        p->DoSend(packet, size);
    }
}

void Server::SendAllPlayerInRoom(void* packet, int size, int roomID)
{
    for (Player* p : GetRooms()[roomID]->GetPlayerList()) {
        if (p == nullptr) continue;
        p->DoSend(packet, size);
    }
}

void Server::SendAllPlayerInRoomExceptSender(void* packet, int size, int sessionID)
{
    Player* player = dynamic_cast<Player*>(GetSessions()[sessionID]);
    int roomID = player->GetRoomID();
    for (Player* p : GetRooms()[roomID]->GetPlayerList()) {
        if (p == nullptr) continue;
        if (p->GetSessionID() == sessionID) continue;
        p->DoSend(packet, size);
    }
}

void Server::SendPlayerAdd(int sessionID, int destination)
{
    mBuilder.Clear();
    Player* player = dynamic_cast<Player*>(GetSessions()[sessionID]);
    int inGameID = player->GetInGameID();
    int roomID = player->GetRoomID();
    mBuilder.Finish(PacketTable::PlayerTable::CreatePlayerAdd(mBuilder, inGameID));
    std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYERADD, mBuilder.GetBufferPointer(), mBuilder.GetSize());

    GetSessions()[destination]->DoSend(send_buffer.data(), send_buffer.size());
}

void Server::SendPlayerGameInfo(int sessionID)
{
    mBuilder.Clear();
    Player* player = dynamic_cast<Player*>(GetSessions()[sessionID]);
    int inGameID = player->GetInGameID();

    int roomID = player->GetRoomID();
    mBuilder.Finish(PacketTable::PlayerTable::CreatePlayerGameInfo(mBuilder, inGameID, roomID));
    std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYERGAMEINFO, mBuilder.GetBufferPointer(), mBuilder.GetSize());

    GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void Server::SendHeartBeatPacket(int sessionID)
{
    mBuilder.Clear();
    long long currTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    mBuilder.Finish(PacketTable::UtilitiesTable::CreateHeartBeat(mBuilder, currTime));

    std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_HEARTBEAT, mBuilder.GetBufferPointer(), mBuilder.GetSize());
    GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void Server::SendObjectDropPacket(int roomID)
{
    // 임시 (게임 데이터 연동안함)

    mBuilder.Clear();

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> x_distrib(0, 19);
    std::uniform_int_distribution<> y_distrib(0, 9);
    std::uniform_int_distribution<> id_distrib(0, 9);

    auto pos = PacketTable::ObjectTable::CreateVec2(mBuilder, x_distrib(gen), y_distrib(gen));
    mBuilder.Finish(PacketTable::ObjectTable::CreateObjectDrop(mBuilder, pos, id_distrib(gen)));

    std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_OBJECTDROP, mBuilder.GetBufferPointer(), mBuilder.GetSize());

    SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void Server::StartHeartBeat(int sessionID)
{
    GetSessions()[sessionID]->SetIsHeartbeatAck(false);
    SendHeartBeatPacket(sessionID);
    PushEventHeartBeat(mTimer, sessionID);
}
