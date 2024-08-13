#pragma once
#include <random>
#include "Server.h"
#include "../Thread/WorkerThread/WorkerThread.h"
#include "../Thread/TimerThread/TimerThread.h"
#include "../Thread/TestThread/TestThread.h"
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
    mTableManager = pTableManager;

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

    mPacketMaker = new PacketMaker;
    // Thread Create
    mTimer = new Timer;
    mTimer->Init(mHcp);
    mTimerThread = std::thread(&Timer::Main, mTimer);

    SYSTEM_INFO si;
    GetSystemInfo(&si);
    for (int i = 0; i < (int)si.dwNumberOfProcessors; ++i) {
        WorkerThread* pWorkerThreadRef = new WorkerThread(this);
        mWorkerThreads.emplace_back(std::thread(&WorkerThread::RunWorker, pWorkerThreadRef));
    }

#ifdef RunTest
    TestThread* pTestThreadRef = new TestThread(this, mTimer);
    mTestThread = std::thread(&TestThread::RunWorker, pTestThreadRef);
#endif
    DEBUGMSGNOPARAM("Thread Ready\n");
}

void Server::ThreadJoin()
{
    for (auto& th : mWorkerThreads) {
        th.join();
    }
    mTimerThread.join();
#ifdef RunTest
    mTestThread.join();
#endif
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
    Player* player = dynamic_cast<Player*>(GetSessions()[sessionID]);
    int inGameID = player->GetInGameID();
    int roomID = player->GetRoomID();
    std::vector<uint8_t> send_buffer = mPacketMaker->MakePlayerAdd(inGameID);

    GetSessions()[destination]->DoSend(send_buffer.data(), send_buffer.size());
}

void Server::SendPlayerGameInfo(int sessionID)
{
    Player* player = dynamic_cast<Player*>(GetSessions()[sessionID]);
    int inGameID = player->GetInGameID();
    int roomID = player->GetRoomID();
    std::vector<uint8_t> send_buffer = mPacketMaker->MakePlayerGameInfo(inGameID, roomID);

    GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void Server::SendHeartBeatPacket(int sessionID)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeHeartBeatPacket();
    GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void Server::SendObjectDropPacket(int roomID, int spawnCount)
{
    std::vector<std::vector<int>> beforeMap = mRooms[roomID]->GetMap().GetStructure();

    for (int i = 0; i < spawnCount; ++i) {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution<> x_distrib(0, 19);
        std::uniform_int_distribution<> y_distrib(0, 9);
        std::uniform_int_distribution<> id_distrib(0, 1);

        std::vector<std::vector<int>>& afterMap = mRooms[roomID]->GetMap().GetStructure();
        int posX = 0, posY = 0, type = id_distrib(gen);
        while (1) {
            posX = x_distrib(gen);
            posY = y_distrib(gen);
            if (afterMap[posY][posX] > 0 && beforeMap[posY][posX] == afterMap[posY][posX]) {
                afterMap[posY][posX]++;
                break;
            }
        }

        std::vector<uint8_t> send_buffer = mPacketMaker->MakeObjectDropPacket(posX, posY, type);

        SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
    }
}

void Server::SendBombSpawnPacket(int roomID, int spawnCount)
{
    for (int i = 0; i < spawnCount; ++i) {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution<> x_distrib(0, 19);
        std::uniform_int_distribution<> y_distrib(0, 9);
        int posX = x_distrib(gen), posY = y_distrib(gen);

        std::vector<uint8_t> send_buffer = mPacketMaker->MakeBombSpawnPacket(posX, posY);

        SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
    }

}

void Server::SendLifeReducePacket(int team, int lifeCount, int roomID) {
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeLifeReducePacket(team, lifeCount);
    SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void Server::StartHeartBeat(int sessionID)
{
    GetSessions()[sessionID]->SetIsHeartbeatAck(false);
    SendHeartBeatPacket(sessionID);
    PushEventHeartBeat(mTimer, sessionID);
}

void Server::StartGame(int roomID)
{
    // Activate New Room
    GetRooms()[roomID]->Init(roomID, GetTableManager()->getFITH_Data()[GameCode::FITH_Team_battle_Three].Team_Life_Count);
    GetRooms()[roomID]->SetGameMode(GameCode::FITH_Team_battle_Three);
    GetRooms()[roomID]->InitMap(GetTableManager()->getMapData()[TEST]);
    GetRooms()[roomID]->SetPlayerLimit(6); // юс╫ц

    // Player Add Into New Room
    for (Session* s : GetSessions()) {
        if (GetRooms()[roomID]->GetPlayerCnt() == GetRooms()[roomID]->GetPlayerLimit()) {
            break;
        }
        if (s->GetState() == eSessionState::ST_ACCEPTED) {
            Player* p = dynamic_cast<Player*>(s);
            int sessionID = p->GetSessionID();
            bool addPlayerOk = GetRooms()[roomID]->addPlayer(p);
            if (addPlayerOk == false) {
                std::cout << "AddPlayer fail: Already Player Max\n";
            }
            else {
                GetRooms()[TESTROOM]->AddPlayerCnt();
                SendPlayerGameInfo(sessionID);
            }
        }
    }

    // Send Each Player's Info
    for (Player* p :GetRooms()[roomID]->GetPlayerList()) {
        if (p == nullptr) continue;
        for (Player* other : GetRooms()[roomID]->GetPlayerList()) {
            if (other == nullptr) continue;
            SendPlayerAdd(p->GetSessionID(), other->GetSessionID());
        }
    }

    // Push Event
    GameCode gameCode = GetRooms()[roomID]->GetGameMode();
    int eventTime = GetTableManager()->getFITH_Data()[gameCode].Block_Spawn_Time;
    PushEventObjectDrop(mTimer, roomID, eventTime);
    PushEventBombSpawn(mTimer, roomID, GetTableManager()->getFITH_Data()[gameCode].Bomb_Spawn_Time);

    GetRooms()[roomID]->SetStartTime(std::chrono::system_clock::now());
}
