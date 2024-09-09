#pragma once
#include <random>
#include "Server.h"
#include "../Thread/WorkerThread/WorkerThread.h"
#include "../Thread/TimerThread/TimerThread.h"
#include "../Thread/TestThread/TestThread.h"
#include "../TableManager/TableManager.h"
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
    delete mTimer;
    delete mTableManager;
    delete mPacketMaker;
    delete mPacketSender;

    delete mTestThreadRef;
    for (WorkerThread* pWorkerThreadRef : mWorkerThreadRefs) {
        delete pWorkerThreadRef;
    }
}

int Server::SetSessionID()
{
    for (int i = STARTKEY; i < MAXSESSION; ++i) {
        auto session = GetSessions()[i];
        if (session == nullptr) continue;
        session->GetStateLock().lock();
        if (eSessionState::ST_FREE == session->GetState()) {
            session->SetState(eSessionState::ST_ACCEPTED);
            session->GetStateLock().unlock();
            return i;
        }
        session->GetStateLock().unlock();
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
            room->SetState(eRoomState::RS_INGAME);
            room->GetStateLock().unlock();
            return i;
        }
        room->GetStateLock().unlock();
    }
    DEBUGMSGNOPARAM("Set Room ID Error\n");
    return INVALIDKEY;
}

void Server::Disconnect(int key)
{
    Player* player = dynamic_cast<Player*>(GetSessions()[key]);
    if (player->GetState() == eSessionState::ST_FREE) {
        return;
    }

    // Delete Player In Room
    if (player->GetState() == eSessionState::ST_INGAME) {
        int roomID = player->GetRoomID();
        int inGameID = player->GetInGameID();
        if (roomID = player->GetRoomID() != INVALIDKEY) {
            mRooms[roomID]->DeletePlayer(inGameID);
        }
        if (inGameID == mRooms[roomID]->GetHostID()) {
            int newHostSessionID = mRooms[roomID]->ChangeHost();
            mPacketSender->SendGameHostChange(newHostSessionID);
        }
    }
    player->Disconnect();
}

void Server::Run(class TableManager* pTableManager, class DB* pDB)
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
    mPacketSender = new PacketSender(this, mPacketMaker);
    // Thread Create
    mTimer = new Timer;
    mTimer->Init(mHcp);
    mTimerThread = std::thread(&Timer::Main, mTimer);

    SYSTEM_INFO si;
    GetSystemInfo(&si);
    for (int i = 0; i < (int)si.dwNumberOfProcessors; ++i) {
        WorkerThread* pWorkerThreadRef = new WorkerThread(this);
        mWorkerThreadRefs.push_back(pWorkerThreadRef);
        mWorkerThreads.emplace_back(std::thread(&WorkerThread::RunWorker, pWorkerThreadRef));
    }

#ifdef RunTest
    mTestThreadRef = new TestThread(this, mTimer);
    mTestThread = std::thread(&TestThread::RunWorker, mTestThreadRef);
#endif
    DEBUGMSGNOPARAM("Thread Ready\n");
    // matching start
    PushEventGameMatching(mTimer);

    for (auto& th : mWorkerThreads) {
        th.join();
    }
    mTimerThread.join();
#ifdef RunTest
    mTestThread.join();
#endif
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
    mRooms[roomID]->GetPlayerListLock().lock_shared();
    for (Player* p : GetRooms()[roomID]->GetPlayerList()) {
        if (p == nullptr) continue;
        p->DoSend(packet, size);
    }
    mRooms[roomID]->GetPlayerListLock().unlock_shared();
}

void Server::SendAllPlayerInRoom(void* packet, int size, int roomID)
{
    mRooms[roomID]->GetPlayerListLock().lock_shared();
    for (Player* p : GetRooms()[roomID]->GetPlayerList()) {
        if (p == nullptr) continue;
        p->DoSend(packet, size);
    }
    mRooms[roomID]->GetPlayerListLock().unlock_shared();
}

void Server::SendAllPlayerInRoomExceptSender(void* packet, int size, int sessionID)
{
    Player* player = dynamic_cast<Player*>(GetSessions()[sessionID]);
    if (player == nullptr) {
        return;
    }
    int roomID = player->GetRoomID();
    mRooms[roomID]->GetPlayerListLock().lock_shared();
    for (Player* p : GetRooms()[roomID]->GetPlayerList()) {
        if (p == nullptr) continue;
        if (p->GetSessionID() == sessionID) continue;
        p->DoSend(packet, size);
    }
    mRooms[roomID]->GetPlayerListLock().unlock_shared();
}

std::set<Vector3f> Server::SetObjectSpawnPos(int roomID, int spawnCount)
{
    /*int RedLife = mRooms[roomID]->GetTeams()[(int)TeamCode::RED].GetLife();
    int BlueLife = mRooms[roomID]->GetTeams()[(int)TeamCode::BLUE].GetLife();*/
    std::vector<std::pair<int, int>>& spawnPoses = mRooms[roomID]->GetMap()->GetObjectSpawnIndexes();
   /* if (RedLife > BlueLife) {
        spawnPoses = mRooms[roomID]->GetMap()->GetBlueObjectSpawnIndexes();
    }
    else if (RedLife < BlueLife) {
        spawnPoses = mRooms[roomID]->GetMap()->GetRedObjectSpawnIndexes();
    }*/

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<>idx_distrib(0, spawnPoses.size() - 1);

    std::set<Vector3f> unique_pos;

    Room* room = mRooms[roomID];
    std::vector<Object*> object_list;
    room->GetAllObjects(object_list);

    while (unique_pos.size() < spawnCount) {
        int idx = idx_distrib(gen);
        Vector3f pos = ConvertVec2iToVec3f(spawnPoses[idx].first, spawnPoses[idx].second);
       /* if (RedLife == BlueLife) {
            pos.x = 20;
        }*/
        int invalid_pos_cnt = 0;
        bool invalid_pos = false;
        for (int i = 0; i < MAXOBJECT; ++i) {
            if (object_list[i] == nullptr) {
                continue;
            }
            if (object_list[i]->GetIsGrabbed() == true) {
                continue;
            }
            if (pos.x == object_list[i]->GetPosition().x
                && pos.y == object_list[i]->GetPosition().y) {
                invalid_pos = true;
                invalid_pos_cnt++;
                break;
            }
        }
        if (invalid_pos == false) {
            unique_pos.emplace(pos);
        }

        if (invalid_pos_cnt >= spawnPoses.size()) {
            break;
        }
    }

    return unique_pos;
}

void Server::StartHeartBeat(int sessionID)
{
    GetSessions()[sessionID]->SetIsHeartbeatAck(false);
    mPacketSender->SendHeartBeatPacket(sessionID);
    PushEventHeartBeat(mTimer, sessionID);
}

int Server::CreateNewRoom(int playerCount, GameMode gameMode)
{
    int roomID = SetRoomID();
    if (roomID == INVALIDKEY) {
        std::cout << "Fali Create New Room\n";
        return INVALIDKEY;
    }
    Room* room = GetRooms()[roomID];
    room->Init(roomID, GetTableManager()-> GetGameModeData()[gameMode]->Team_Life_Count);
    room->SetGameMode(gameMode);
    room->InitMap(GetTableManager()->GetMapData()[MapCode::TEST]);

    room->SetPlayerLimit(playerCount); // 임시

    return roomID;
}

void Server::MatchingComplete(int roomID, int playerCnt, std::vector<Player*>& players)
{
    Room* room = GetRooms()[roomID];

    int tFlag = 0;

    // Player Add Into New Room
    for (Player* player : players) {
        if (room->GetPlayerCnt() == room->GetPlayerLimit()) {
            break;
        }
        player->GetStateLock().lock();
        if (player->GetState() == eSessionState::ST_MATCHWAITING) {
            // 임시
            player->SetTeam(tFlag % 2);
            player->SetHP(GetTableManager()->GetCharacterStats()[(int)eCharacterType::CT_TEST]->hp);
            tFlag++;
            //

            int sessionID = player->GetSessionID();
            bool AddPlayerOk = room->AddPlayer(player);
            if (AddPlayerOk == false) {
                std::cout << "AddPlayer fail: Already Player Max\n";
            }
            else {
                if (room->GetHostID() == INVALIDKEY) {
                    room->SetHost(player->GetInGameID());
                }
                room->AddPlayerCnt();
                mPacketSender->SendGameMatchingResponse(sessionID);
            }
        }
        player->GetStateLock().unlock();
    }
}

void Server::StartGame(int roomID)
{
    Room* room = GetRooms()[roomID];
    if (room->SetIsRun(true) == true) {

        mPacketSender->SendGameStart(roomID);

        // Push Event
        long long roomCode = room->GetRoomCode();
        GameMode GameMode = room->GetGameMode();
        int eventTime = GetTableManager()-> GetGameModeData()[GameMode]->Block_Spawn_Time;
        PushEventBlockDrop(mTimer, roomID, roomCode, eventTime);
        PushEventBombSpawn(mTimer, roomID, roomCode, GetTableManager()-> GetGameModeData()[GameMode]->Bomb_Spawn_Time);
        PushEventWeaponSpawn(mTimer, roomID, roomCode, GetTableManager()-> GetGameModeData()[GameMode]->Weapon_Spawn_Time);
        PushEventRemainTimeSync(mTimer, roomID, roomCode);
        PushEventTimeOverCheck(mTimer, roomID, roomCode);

        GetRooms()[roomID]->SetStartTime(std::chrono::system_clock::now());
    }
}

void Server::CheckGameEnd(int roomID)
{
    Room* room = GetRooms()[roomID];
    int teamCnt = 2; // 팀 수 (임시 나중에 게임 데이터로 읽어야 함)
    int loseTeamCnt = 0;
    uint8_t winningTeams_flag = DEFAULT_8BITFLAG;
    for (auto iter = room->GetTeams().begin(); iter != room->GetTeams().end(); ++iter) {
        if (iter->second.GetLife() <= 0) {
            loseTeamCnt++;
        }
        else {
            winningTeams_flag = winningTeams_flag | (1 << iter->first);
        }
    }

    if (loseTeamCnt = teamCnt - 1) {
        if (room->SetIsRun(false) == true) {
            mPacketSender->SendGameEndPacket(roomID, winningTeams_flag);
            room->GetPlayerListLock().lock_shared();
            for (auto player : room->GetPlayerList()) {
                if (player == nullptr) continue;
                player->GetStateLock().lock();
                player->SetState(eSessionState::ST_ACCEPTED);
                player->GetStateLock().unlock();
            }
            room->GetPlayerListLock().unlock_shared();

            // 종료하자마자 바로 초기화 하는데 나중에 어떻게 해야할지 고민해야할듯
            GetRooms()[roomID]->Reset();

            std::cout << "Game End - " << roomID << std::endl;
        }
    }
}

void Server::TimeoverGameEnd(int roomID) {
    Room* room = GetRooms()[roomID];

    uint8_t winningTeams_flag = DEFAULT_8BITFLAG;
    int maxLife = 0;

    for (auto iter = room->GetTeams().begin(); iter != room->GetTeams().end(); ++iter) {
        int life = iter->second.GetLife();
        if (maxLife < life) {
            winningTeams_flag = DEFAULT_8BITFLAG | (1 << iter->first);
            maxLife = life;
        }
        else if (maxLife == life) {
            winningTeams_flag = winningTeams_flag | (1 << iter->first);
        }
    }

    if (room->SetIsRun(false) == true) {
        mPacketSender->SendGameEndPacket(roomID, winningTeams_flag);
        room->GetPlayerListLock().lock_shared();
        for (auto player : room->GetPlayerList()) {
            if (player == nullptr) continue;
            player->GetStateLock().lock();
            player->Init();
            player->SetState(eSessionState::ST_ACCEPTED);
            player->GetStateLock().unlock();
        }
        room->GetPlayerListLock().unlock_shared();

        // 종료하자마자 바로 초기화 하는데 나중에 어떻게 해야할지 고민해야할듯
        GetRooms()[roomID]->Reset();

        std::cout << "Game End - " << roomID << std::endl;
    }
}
