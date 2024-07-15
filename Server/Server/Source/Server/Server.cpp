#pragma once
#include "Server.h"
#include "../Thread/WorkerThread/WorkerThread.h"
//#include "../Thread/TimerThread/TimerThread.h"

int Server::SetKey()
{
    for (int i = STARTKEY; i < MAXPLAYER; ++i) {
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

    DEBUGMSGONEPARAM("Lobby Disconnect: %d\n", key);
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



    for (int i = 0; i < MAXPLAYER; ++i) {
        mSessions[i] = new Session();
    }

    pDB->Init();

    // Thread Create
    pPacketManager->Init();

    WorkerThread* pWorkerThreadRef = new WorkerThread(this, pPacketManager);
    //Timer* pTimer = new Timer;
    //pTimer->Init(mHcp);

    SYSTEM_INFO si;
    GetSystemInfo(&si);
    for (int i = 0; i < (int)si.dwNumberOfProcessors; ++i)
        mWorkerThreads.emplace_back(thread(&WorkerThread::RunWorker, pWorkerThreadRef));
    //thread timerThread = thread(&Timer::TimerMain, pTimer);


    DEBUGMSGNOPARAM("Thread Ready\n");
}

void Server::ThreadRun()
{
    for (auto& th : mWorkerThreads)
        th.join();
}
