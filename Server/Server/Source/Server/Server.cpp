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
        int roomID;
        int inGameID = player->GetInGameID();
        if (roomID = player->GetRoomID() != INVALIDKEY) {
            mRooms[roomID]->DeletePlayer(inGameID);
        }
        if (inGameID == mRooms[roomID]->GetHostID()) {
            int newHostSessionID = mRooms[roomID]->ChangeHost();
            SendGameHostChange(newHostSessionID);
        }
    }
    player->Disconnect();
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
    //PushEventGameMatching(mTimer);
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

void Server::SendPlayerAdd(int sessionID, int destination)
{
    Player* player = dynamic_cast<Player*>(GetSessions()[sessionID]);
    if (player == nullptr) {
        return;
    }
    int inGameID = player->GetInGameID();
    int roomID = player->GetRoomID();
    std::vector<uint8_t> send_buffer = mPacketMaker->MakePlayerAdd(inGameID);

    GetSessions()[destination]->DoSend(send_buffer.data(), send_buffer.size());
}

void Server::SendGameMatchingResponse(int sessionID)
{
    Player* player = dynamic_cast<Player*>(GetSessions()[sessionID]);
    if (player == nullptr) {
        return;
    }
    int inGameID = player->GetInGameID();
    int roomID = player->GetRoomID();
    int team = player->GetTeam();
    Room* room = mRooms[roomID];
    std::vector<uint8_t> send_buffer;
    if (inGameID == room->GetHostID()) {
        send_buffer = mPacketMaker->MakeGameMatchingResponsePacket(inGameID, roomID, team, room->GetGameMode(), room->GetPlayerLimit(), true);
    }
    else {
        send_buffer = mPacketMaker->MakeGameMatchingResponsePacket(inGameID, roomID, team, room->GetGameMode(), room->GetPlayerLimit());
    }

    GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void Server::SendGameStart(int roomID)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeGameStartPacket(roomID);
    SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void Server::SendAllPlayerReady(int roomID)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeAllPlayerReadyPacket();
    SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void Server::SendHeartBeatPacket(int sessionID)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeHeartBeatPacket();
    GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void Server::SendBlockDropPacket(int roomID, int spawnCount)
{
    std::vector<std::pair<int, int>>& spawnPoses = mRooms[roomID]->GetMap().GetBlockDropIndexes();
    GameCode gameMode = mRooms[roomID]->GetGameMode();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> idx_distrib(0, spawnPoses.size()-1);
    std::uniform_int_distribution<> type_distrib(0, 1);

    std::set<int> unique_idx;

    while (unique_idx.size() < spawnCount) {
        int idx = idx_distrib(gen);
        unique_idx.emplace(idx);
    }

    for (const auto& idx : unique_idx) {
        std::vector<uint8_t> send_buffer = mPacketMaker->MakeBlockDropPacket(spawnPoses[idx].first, spawnPoses[idx].second, type_distrib(gen));
        SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
    }
}

void Server::SendBombSpawnPacket(int roomID, int spawnCount)
{
    std::vector<std::pair<int, int>>& spawnPoses = mRooms[roomID]->GetMap().GetBombSpawnIndexes();
    Room* room = mRooms[roomID];
    GameCode gameMode = room->GetGameMode();
    int explosionInterval = mTableManager->getFITH_Data()[gameMode].Bomb_Delay_Time;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> idx_distrib(0, spawnPoses.size() - 1);

    std::set<int> unique_idx;

    room->GetObjectListLock().lock_shared();
    std::array<Object*, MAXOBJECT>& object_list = room->GetObjects();
    room->GetObjectListLock().unlock_shared();
    while (unique_idx.size() < spawnCount) {
        int idx = idx_distrib(gen);
        int x = spawnPoses[idx].first;
        int y = spawnPoses[idx].second;
        bool invalid_pos = false;
        for (int i = 0; i < MAXOBJECT; ++i) {
            if (object_list[i] == nullptr) {
                continue;
            }
            if (x == object_list[i]->GetPosition().x 
                && y == object_list[i]->GetPosition().y) {
                invalid_pos = true;
                break;
            }
        }
        if (invalid_pos == false) {
            unique_idx.emplace(idx);
        }
    }
    for (const auto& idx : unique_idx) {
        int x = spawnPoses[idx].first;
        int y = spawnPoses[idx].second;
        int bombid = room->AddBomb(new Bomb, Vector3f(x, y, 0));
        if (bombid == INVALIDKEY) continue;
        std::vector<uint8_t> send_buffer = mPacketMaker->MakeBombSpawnPacket(x, y, bombid);
        SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
        PushEventBombExplosion(mTimer, roomID, bombid, room->GetRoomCode(), explosionInterval);
    }
}


void Server::SendBombExplosionPacket(int roomID, int bombID)
{
    mRooms[roomID]->GetObjectListLock().lock_shared();
    Object* object = mRooms[roomID]->GetObjects()[bombID];
    if (object == nullptr) {
        mRooms[roomID]->GetObjectListLock().unlock_shared();
        return;
    }
    Vector3f pos = object->GetPosition();
    mRooms[roomID]->GetObjectListLock().unlock_shared();
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeBombExplosionPacket(bombID, pos);
    SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void Server::SendLifeReducePacket(int team, int lifeCount, int roomID) {
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeLifeReducePacket(team, lifeCount);
    SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void Server::SendRemainTimeSync(int roomID)
{
    TIMEPOINT startTime = GetRooms()[roomID]->GetStartTime();
    GameCode gameCode = GetRooms()[roomID]->GetGameMode();
    int playTime = mTableManager->getFITH_Data()[gameCode].Play_Time;
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeRemainTimeSyncPacket(roomID, startTime, playTime);
    SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void Server::SendGameStartPacket(int roomID)
{
    long startTime = 0;
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeGameStartPacket(roomID, startTime);
    SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void Server::SendGameEndPacket(int roomID, uint8_t winningTeams_flag)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeGameEndPacket(winningTeams_flag);
    SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void Server::SendGameHostChange(int sessionID)
{
    if (sessionID == INVALIDKEY) {
        return;
    }
    Player* player = dynamic_cast<Player*>(GetSessions()[sessionID]);
    if (player == nullptr) {
        return;
    }
    int inGameID = player->GetInGameID();
    int roomID = player->GetRoomID();
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeGameHostChangePacket(inGameID, roomID);

    player->DoSend(send_buffer.data(), send_buffer.size());
}

void Server::SendPlayerRespawn(int inGameID, int roomID)
{
    mRooms[roomID]->GetPlayerListLock().lock_shared();
    Player* player = dynamic_cast<Player*>(mRooms[roomID]->GetPlayerList()[inGameID]);
    mRooms[roomID]->GetPlayerListLock().unlock_shared();
    if (player == nullptr) {
        return;
    }
    int team = player->GetTeam();
    std::vector<std::pair<int, int>>& spawnPoses = mRooms[roomID]->GetMap().GetPlayerSpawnIndexes(team);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> idx_distrib(0, spawnPoses.size() - 1);
    int idx = idx_distrib(gen);
    int posX = spawnPoses[idx].first;
    int posY = spawnPoses[idx].second;

    std::vector<uint8_t> send_buffer = mPacketMaker->MakePlayerRespawnPacket(inGameID, roomID, posX, posY);
    SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void Server::StartHeartBeat(int sessionID)
{
    GetSessions()[sessionID]->SetIsHeartbeatAck(false);
    SendHeartBeatPacket(sessionID);
    PushEventHeartBeat(mTimer, sessionID);
}

void Server::MatchingComplete(int roomID, int playerCnt)
{
    Room* room = GetRooms()[roomID];
    room->Init(roomID, GetTableManager()->getFITH_Data()[GameCode::FITH_Team_battle_Three].Team_Life_Count);
    room->SetGameMode(GameCode::FITH_Team_battle_Three);
    room->InitMap(GetTableManager()->getMapData()[TEST]);

    room->SetPlayerLimit(playerCnt); // 임시

    int tFlag = 0;

    // Player Add Into New Room
    for (Session* s : GetSessions()) {
        if (room->GetPlayerCnt() == room->GetPlayerLimit()) {
            break;
        }
        s->GetStateLock().lock();
        if (s->GetState() == eSessionState::ST_GAMEREADY) {
            Player* p = dynamic_cast<Player*>(s);

            // 임시
            p->SetTeam(tFlag % 2);
            tFlag++;
            //

            int sessionID = p->GetSessionID();
            bool AddPlayerOk = room->AddPlayer(p);
            if (AddPlayerOk == false) {
                std::cout << "AddPlayer fail: Already Player Max\n";
            }
            else {
                if (room->GetHostID() == INVALIDKEY) {
                    room->SetHost(p->GetInGameID());
                }
                room->AddPlayerCnt();
                SendGameMatchingResponse(sessionID);
            }
        }
        s->GetStateLock().unlock();
    }

    room->SetPlayerLimit(room->GetPlayerCnt()); // 임시

    room->GetPlayerListLock().lock_shared();
    for (Player* p : room->GetPlayerList()) {
        if (p == nullptr) continue;
        for (Player* other : room->GetPlayerList()) {
            if (other == nullptr) continue;
            SendPlayerAdd(p->GetSessionID(), other->GetSessionID());
        }
    }
    room->GetPlayerListLock().unlock_shared();
}

void Server::StartGame(int roomID)
{
    Room* room = GetRooms()[roomID];
    if (room->SetIsRun(true) == true) {

        SendGameStart(roomID);

        // Push Event
        long long roomCode = room->GetRoomCode();
        GameCode gameCode = room->GetGameMode();
        int eventTime = GetTableManager()->getFITH_Data()[gameCode].Block_Spawn_Time;
        PushEventBlockDrop(mTimer, roomID, roomCode, eventTime);
        PushEventBombSpawn(mTimer, roomID, roomCode, GetTableManager()->getFITH_Data()[gameCode].Bomb_Spawn_Time);
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
            SendGameEndPacket(roomID, winningTeams_flag);
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
        SendGameEndPacket(roomID, winningTeams_flag);
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
    }
}
