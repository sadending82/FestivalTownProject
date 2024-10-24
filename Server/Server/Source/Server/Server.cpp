#pragma once
#include <iostream>
#include <fstream>
#include <set>
#include <random>
#include "Server.h"
#include "../Thread/WorkerThread/WorkerThread.h"
#include "../Thread/TimerThread/TimerThread.h"
#include "../Thread/TestThread/TestThread.h"
#include "../Event/Event.h"

Server::Server()
{
}

Server::~Server()
{
    for (int i = 0; i < MAXSESSION; ++i) {
        delete mSessions[i];
    }
    for (int i = 0; i < MAXROOM; ++i) {
        delete mRooms[i];
    }
    delete mDB;
    delete mTimer;
    delete mPacketManager;
    delete mPacketMaker;
    delete mPacketSender;
    delete mMatchingManager;

    delete mTestThreadRef;
    for (WorkerThread* pWorkerThreadRef : mWorkerThreadRefs) {
        delete pWorkerThreadRef;
    }

    for (auto pair : mGameManagers) {
        delete pair.second;
    }
}

bool Server::ReadConfig()
{
    std::string txt;

    std::ifstream file("Config/Config.txt");
    if (!file) return false;
    int line = 0;

    while (line < 4 && std::getline(file, txt)) {
        if (line == 0) {
            std::string tmp = txt.substr(5);
            mOdbc.assign(tmp.begin(), tmp.end());
        }
        if (line == 1) {
            std::string tmp = txt.substr(3);
            mDB_ID.assign(tmp.begin(), tmp.end());
        }
        if (line == 2) {
            std::string tmp = txt.substr(3);
            mDB_Password.assign(tmp.begin(), tmp.end());
        }
        if (line == 3) {
            std::string tmp = txt.substr(5);
            mMode = static_cast<SERVER_MODE>(stoi(tmp));
        }
        line++;
    }
    return true;
}

int Server::SetSessionID()
{
    for (int i = STARTKEY; i < MAXSESSION; ++i) {
        auto session = GetSessions()[i];
        if (session == nullptr) continue;
        session->GetSessionStateLock().lock();
        if (eSessionState::ST_FREE == session->GetSessionState()) {
            session->SetSessionState(eSessionState::ST_ACCEPTED);
            session->GetSessionStateLock().unlock();
            return i;
        }
        session->GetSessionStateLock().unlock();
    }
    DEBUGMSGNOPARAM("Set Session ID Error\n");
    return INVALIDKEY;
}

int Server::SetRoomID()
{
    for (int i = STARTKEY; i < MAXROOM; ++i) {
        auto room = GetRooms()[i];
        if (room == nullptr) continue;
        room->GetStateLock().lock();
        if (eRoomState::RS_FREE == room->GetState()) {
            room->Reset();
            room->SetState(eRoomState::RS_INGAME);
            room->GetStateLock().unlock();
            return i;
        }
        room->GetStateLock().unlock();
    }
    DEBUGMSGNOPARAM("Set Room ID Error\n");
    return INVALIDKEY;
}

bool Server::Disconnect(int key)
{
    Player* player = dynamic_cast<Player*>(GetSessions()[key]);

    player->GetSessionStateLock().lock();
    if (player->GetSessionState() == eSessionState::ST_FREE) {
        player->GetSessionStateLock().unlock();
        return false;
    }

    // Delete Player In Room
    if (player->GetSessionState() == eSessionState::ST_INGAME) {
        int roomID = player->GetRoomID();
        int inGameID = player->GetInGameID();

        if (roomID != INVALIDKEY) {
            Room* room = mRooms[roomID];

            GameMode gameMode = room->GetGameMode();

            mGameManagers[gameMode]->DeletePlayer(inGameID, roomID);

            mPacketSender->SendPlayerDelete(roomID, inGameID);

            if (inGameID == room->GetHostID()) {
                int newHostSessionID = room->ChangeHost();
                mPacketSender->SendGameHostChange(newHostSessionID);
            }
        }

    }
    player->GetSessionStateLock().unlock();
    player->Disconnect();

    return true;
}

void Server::Run()
{
    ReadConfig();
    mTableManager = new TableManager;
    mTableManager->ReadAllDataTable();
    mDB = new DB;
    mDB->Connect(mOdbc, mDB_ID, mDB_Password);

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
        mRooms[i] = new Room(this);
    }

    mPacketMaker = new PacketMaker;
    mPacketSender = new PacketSender(this, mPacketMaker);
    mPacketManager = new PacketManager(this, mPacketSender);
    mMatchingManager = new MatchingManager(this);
    mTimer = new Timer;
    mTimer->Init(mHcp);

    // Thread Create

    mTimerThread = std::thread(&Timer::Main, mTimer);

    SYSTEM_INFO si;
    GetSystemInfo(&si);
    for (int i = 0; i < (int)si.dwNumberOfProcessors; ++i) {
        WorkerThread* pWorkerThreadRef = new WorkerThread(this, mPacketManager);
        mWorkerThreadRefs.push_back(pWorkerThreadRef);
        mWorkerThreads.emplace_back(std::thread(&WorkerThread::RunWorker, pWorkerThreadRef));
    }

#ifdef RunTest
    mTestThreadRef = new TestThread(this, mTimer);
    mTestThread = std::thread(&TestThread::RunWorker, mTestThreadRef);
#endif
    SetGameManagers();
    DEBUGMSGNOPARAM("Thread Ready\n");

    switch (mMode) {
    case SERVER_MODE::LIVE: {
        // matching start
        PushEventGameMatching(mTimer);
    }break;
    case SERVER_MODE::TEST: {
        DEBUGMSGNOPARAM("START TEST MODE \n");
        //MakeTestRoom();
    }break;
    }
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

void Server::SetGameManagers()
{
    mGameManagers[GameMode::FITH_Indiv_Battle_2] = new FITH(this, GameMode::FITH_Indiv_Battle_2);
    mGameManagers[GameMode::FITH_Indiv_Battle_3] = new FITH(this, GameMode::FITH_Indiv_Battle_3);
    mGameManagers[GameMode::FITH_Indiv_Battle_5] = new FITH(this, GameMode::FITH_Indiv_Battle_5);
    mGameManagers[GameMode::FITH_Team_Battle_4] = new FITH(this, GameMode::FITH_Team_Battle_4);
    mGameManagers[GameMode::FITH_Team_Battle_6] = new FITH(this, GameMode::FITH_Team_Battle_6);
}

void Server::TableReLoad()
{
    mTableManager->Lock();
    mTableManager->ReadAllDataTable();
    mTableManager->UnLock();
}

void Server::MakeTestRoom()
{
    GameMode gameMode = GameMode::FITH_Team_Battle_6;
    GameModeData& modeInfo = mTableManager->GetGameModeData()[gameMode];
    Room* room = mRooms[TESTROOM];
    room->Init(TESTROOM, gameMode, modeInfo);
    room->InitMap(&mTableManager->GetMapData()[MapCode::TEST]);
    room->SetState(eRoomState::RS_INGAME);

    // Push Event
    long long roomCode = room->GetRoomCode();

    PushEventBlockDrop(mTimer, TESTROOM, roomCode, modeInfo.Block1_Spawn_Index, modeInfo.Block1_Spawn_Time);
    PushEventBlockDrop(mTimer, TESTROOM, roomCode, modeInfo.Block2_Spawn_Index, modeInfo.Block2_Spawn_Time);

    PushEventBombSpawn(mTimer, TESTROOM, roomCode, modeInfo.Bomb_Spawn_Time);

    PushEventWeaponSpawn(mTimer, TESTROOM, roomCode, modeInfo.Weapon1_Spawn_Index, modeInfo.Weapon1_Spawn_Time);
    PushEventWeaponSpawn(mTimer, TESTROOM, roomCode, modeInfo.Weapon2_Spawn_Index, modeInfo.Weapon2_Spawn_Time);

    GetRooms()[TESTROOM]->SetStartTime(std::chrono::system_clock::now());
}

void Server::SendAllPlayerInRoomBySessionID(void* packet, int size, int sessionID)
{
    int roomID = dynamic_cast<Player*>(GetSessions()[sessionID])->GetRoomID();
    for (const auto& id : GetRooms()[roomID]->GetPlayerList()) {
        int session_id = id.load();
        if (session_id == INVALIDKEY) continue;
        Player* player = dynamic_cast<Player*>(mSessions[session_id]);
        if (player->GetIsBot()) continue;
        player->DoSend(packet, size);
    }
}

void Server::SendAllPlayerInRoom(void* packet, int size, int roomID)
{
    for (const auto& sID : GetRooms()[roomID]->GetPlayerList()) {
        int session_id = sID.load();
        if (session_id == INVALIDKEY) continue;
        Player* player = dynamic_cast<Player*>(mSessions[session_id]);
        if (player->GetIsBot()) continue;
        player->DoSend(packet, size);
    }
}

void Server::SendAllPlayerInRoomExceptSender(void* packet, int size, int sessionID)
{
    Player* player = dynamic_cast<Player*>(GetSessions()[sessionID]);
    if (player == nullptr) {
        return;
    }
    int roomID = player->GetRoomID();
    if (roomID == INVALIDKEY) {
        return;
    }
    for (const auto& id : GetRooms()[roomID]->GetPlayerList()) {
        int session_id = id.load();
        if (session_id == INVALIDKEY) continue;
        if (session_id == sessionID) continue;
        Player* player = dynamic_cast<Player*>(mSessions[session_id]);
        if (player->GetIsBot()) continue;
        player->DoSend(packet, size);
    }
}

void Server::StartHeartBeat(int sessionID)
{
    GetSessions()[sessionID]->SetIsHeartbeatAck(false);
    mPacketSender->SendHeartBeatPacket(sessionID);
    PushEventHeartBeat(mTimer, sessionID);
}

std::pair<bool, UserInfo> Server::UserLogin(const char* accountID, const char* accountPassword, const int sessionID)
{
    if (mDB->CheckValidateLogin(accountID, accountPassword) == false) {
        return { false, UserInfo() };
    }

    std::pair<bool, UserInfo> result = mDB->SelectUserInfoForLogin(accountID);

    UserInfo& userInfo = result.second;

    if (result.first == false) {
        return result;
    }

    if (userInfo.State == true) {
        for (Session* session : mSessions) {
            if (session == nullptr) {
                continue;
            }

            Player* player = dynamic_cast<Player*>(session);

            if (player->GetUID() == userInfo.UID) {
                player->Disconnect();
                break;
            }
        }
    }
    Player* player = dynamic_cast<Player*>(GetSessions()[sessionID]);
    player->SetUserInfoFromDB(userInfo);

    return { true, userInfo };
}

int Server::CreateNewRoom(GameMode gameMode)
{
    int roomID = SetRoomID();
    if (roomID == INVALIDKEY) {
        std::cout << "Fali Create New Room\n";
        return INVALIDKEY;
    }
    Room* room = GetRooms()[roomID];
    room->Init(roomID, gameMode, mTableManager->GetGameModeData()[gameMode]);
    room->InitMap(&GetTableManager()->GetMapData()[MapCode::TEST]);

    return roomID;
}