#pragma once
#include "Server.h"
#include "../Thread/WorkerThread/WorkerThread.h"
#include "../Thread/TimerThread/TimerThread.h"
#include "../TableManager/TableManager.h"
#include "../PacketManager/PacketManager.h"

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
    auto session = GetSessions()[key];
    if (session->GetState() == eSessionState::ST_FREE) {
        return;
    }
    closesocket(session->GetSocket());
    session->SetState(eSessionState::ST_FREE);

    DEBUGMSGONEPARAM("Disconnect: %d\n", key);
}

void Server::Init(class PacketManager* pPacketManager, class TableManager* pTableManager, class DB* pDB)
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
        mSessions[i] = new Session();
    }
    for (int i = 0; i < MAXROOM; ++i) {
        mRooms[i] = new Room();
    }

    pDB->Init();

    // Thread Create
    pPacketManager->Init(this);
    mTimer = new Timer;
    mTimer->Init(mHcp);

    // 테스트를 위해 임시 방 추가
    mRooms[0]->Init(0);

    SYSTEM_INFO si;
    GetSystemInfo(&si);
    for (int i = 0; i < (int)si.dwNumberOfProcessors; ++i) {
        WorkerThread* pWorkerThreadRef = new WorkerThread(this, pPacketManager);
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
    int roomID = reinterpret_cast<Player*>(GetSessions()[sessionID])->GetRoomID();
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
