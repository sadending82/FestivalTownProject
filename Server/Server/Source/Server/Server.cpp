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
    delete mMatchMakingManager;
    delete mLobbyManager;

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
            session->SetSessionID(i);
            session->SetSessionState(eSessionState::ST_ACCEPTED);
            session->GetSessionStateLock().unlock();
            return i;
        }
        session->GetSessionStateLock().unlock();
    }
    DEBUGMSGNOPARAM("Set Session ID Error\n");
    return INVALIDKEY;
}

int Server::SetroomID()
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

    switch (player->GetSessionState()) {
    case eSessionState::ST_FREE: {
        player->GetSessionStateLock().unlock();
        return false;
    }
    break;
    case eSessionState::ST_MATCHWAITING: {
        mMatchMakingManager->GetMatchingLock().lock();

        eMatchingType matchingType = player->GetMatchingRequestType();

        MATCHING_QUEUE& matchingQueue = mMatchMakingManager->GetMatchingQueue(player->GetMatchingRequestType());

        int top_ID = matchingQueue.begin()->first;
        long long top_requestTime = matchingQueue.begin()->second;

        matchingQueue.erase({ key, player->GetMatchingRequestTime() });

        // 그 다음 최장 대기 유저 기준으로 매칭 시퀀스 갱신
        if (top_ID == key && player->GetMatchingRequestTime() == top_requestTime) {
            mMatchMakingManager->SetMatchingSequence(matchingType, eMatchingSequence::MS_None);
            mMatchMakingManager->UpdateMatchingSequence(matchingType);
        }

        player->SetMatchingRequestTime(0);

        mMatchMakingManager->GetMatchingLock().unlock();
    }
    break;

    case eSessionState::ST_GAMELOADING: {

        int roomID = player->GetRoomID();
        int inGameID = player->GetInGameID();

        if (roomID != INVALIDKEY) {
            Room* room = mRooms[roomID];

            room->GetTeams()[player->GetTeam()].GetMembers().erase(inGameID);
            room->GetPlayerList()[inGameID].store(INVALIDKEY);
            if (room->GetPlayerCnt() > 0) {
                room->SubPlayerCnt();
                room->SetIsPlayerReady(inGameID, false);
            }

            mPacketSender->SendPlayerDelete(roomID, inGameID);

            switch (mMode) {
            case SERVER_MODE::LIVE: {
                mLobbyManager->CheckReadyToGamePlay(room, roomID);
            }break;

            }

            if (inGameID == room->GetHostID()) {
                int newHostSessionID = room->ChangeHost();
                mPacketSender->SendGameHostChange(newHostSessionID);
            }
        }
    }
    break;
    case eSessionState::ST_INGAME: {
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
    break;
    }
    player->Disconnect();
    player->GetSessionStateLock().unlock();

    return true;
}

void Server::Run()
{
    ReadConfig();
    mTableManager = new TableManager;
    mTableManager->ReadAllDataTable();
    mDB = new DB(mTableManager);
    mDB->Connect(mOdbc, mDB_ID, mDB_Password);

    // 오래된 데이터 삭제
    mDB->DeleteUserAttendanceOutdated(AttendanceData_DaysToKeep);

    mLobbyManager = new LobbyManager(this);

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
    mMatchMakingManager = new MatchMakingManager(this);
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

void Server::SendAllPlayerInRoomBySessionID(void* packet, int size, int sessionID)
{
    int roomID = dynamic_cast<Player*>(GetSessions()[sessionID])->GetRoomID();
    for (const auto& id : GetRooms()[roomID]->GetPlayerList()) {
        int session_id = id.load();
        if (session_id == INVALIDKEY) continue;
        Player* player = dynamic_cast<Player*>(mSessions[session_id]);
        if (player->GetIsBot()) continue;
        if (player->GetRoomID() != roomID) continue;
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
        if (player->GetRoomID() != roomID) continue;
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
        if (player->GetRoomID() != roomID) continue;
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

    userInfo.Gold = mDB->SelectUserItemCount(userInfo.UID, 100001);
    userInfo.Dia = mDB->SelectUserItemCount(userInfo.UID, 100002);
    userInfo.Mileage = mDB->SelectUserItemCount(userInfo.UID, 100003);

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
                Disconnect(session->GetSessionID());
                break;
            }
        }
    }
    Player* player = dynamic_cast<Player*>(GetSessions()[sessionID]);
    player->SetUserInfoFromDB(userInfo);

    return { true, userInfo };
}

int Server::CreateNewRoom(GameMode gameMode, int mapIndex, int mapTheme)
{
    if (mTableManager->GetGameModeData()[mapIndex].find(gameMode) == mTableManager->GetGameModeData()[mapIndex].end()) {
        std::cout << "Fali Create New Room\n";
        return INVALIDKEY;
    }
    int roomID = SetroomID();
    if (roomID == INVALIDKEY) {
        std::cout << "Fali Create New Room\n";
        return INVALIDKEY;
    }

    Room* room = GetRooms()[roomID];
    room->Init(roomID, gameMode, mTableManager->GetGameModeOutData()[gameMode], mTableManager->GetGameModeData()[mapIndex][gameMode]);
    room->InitMap(&GetTableManager()->GetMapData()[mapIndex], mapTheme);

    return roomID;
}