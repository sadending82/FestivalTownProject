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

    player->GetDisconnectLock().lock();
    if (player->GetState() == eSessionState::ST_FREE) {
        player->GetDisconnectLock().unlock();
        return;
    }

    // Delete Player In Room
    if (player->GetState() == eSessionState::ST_INGAME) {
        int roomID = player->GetRoomID();
        int inGameID = player->GetInGameID();

        if (roomID != INVALIDKEY) {
            mRooms[roomID]->DeletePlayer(inGameID);

            mPacketSender->SendPlayerDelete(roomID, inGameID);

            if (inGameID == mRooms[roomID]->GetHostID()) {
                int newHostSessionID = mRooms[roomID]->ChangeHost();
                mPacketSender->SendGameHostChange(newHostSessionID);
            }
        }

    }
    player->Disconnect();
    player->GetDisconnectLock().unlock();
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

    mDB = pDB;
    mDB->Init();

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
    if (roomID == INVALIDKEY) {
        return;
    }
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
    std::vector<std::pair<int, int>>& spawnPoses = mRooms[roomID]->GetMap()->GetObjectSpawnIndexes();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<>idx_distrib(0, spawnPoses.size() - 1);

    std::set<Vector3f> unique_pos;
    std::set<int> invalid_pos_index;

    Room* room = mRooms[roomID];

    while (unique_pos.size() < spawnCount) {
        int idx = idx_distrib(gen);
        Vector3f spawnIndex = Vector3f(spawnPoses[idx].first, 0, spawnPoses[idx].second);
        bool invalid_pos = false;

        //// 스폰 위치에 다른 오브젝트가 있는지 체크
        //room->GetBombListLock().lock_shared();
        //for (Bomb* bomb : room->GetBombList()) {
        //    if (bomb == nullptr) {
        //        continue;
        //    }
        //    if (bomb->GetIsGrabbed() == true) {
        //        continue;
        //    }
        //    Vector3f Index = ConvertVec3fToVec2i(bomb->GetPosition());

        //    if (spawnIndex.x == Index.x
        //        && spawnIndex.z == Index.z) {
        //        invalid_pos = true;
        //        invalid_pos_index.insert(idx);
        //        break;
        //    }
        //}
        //room->GetBombListLock().unlock_shared();

        //if (invalid_pos == true) {
        //    continue;
        //}

        //room->GetWeaponListLock().lock_shared();
        //for (Weapon* weapon : room->GetWeaponList()) {
        //    if (weapon == nullptr) {
        //        continue;
        //    }
        //    if (weapon->GetIsGrabbed() == true) {
        //        continue;
        //    }
        //    Vector3f Index = ConvertVec3fToVec2i(weapon->GetPosition());

        //    if (spawnIndex.x == Index.x
        //        && spawnIndex.z == Index.z) {

        //        COUT << "이미 있음;; " << spawnIndex.x << " " << spawnIndex.z << ENDL;

        //        invalid_pos = true;
        //        invalid_pos_index.insert(idx);
        //        break;
        //    }
        //}
        //room->GetWeaponListLock().unlock_shared();

        if (invalid_pos == false) {
            unique_pos.emplace(ConvertVec2iToVec3f(spawnIndex.x, spawnIndex.z));
        }

        //if (invalid_pos_index.size() >= spawnPoses.size()) {
        //    return std::set<Vector3f>();
        //    //break;
        //}
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
    room->Init(roomID, GetTableManager()->GetGameModeData()[gameMode].Life_Count, GetTableManager()->GetGameModeData()[gameMode].Player_Count);
    room->SetGameMode(gameMode);
    room->InitMap(&GetTableManager()->GetMapData()[MapCode::TEST]);

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
            int team = tFlag % 2;
            player->SetTeam(team);
            player->SetChacracterType(eCharacterType::CT_TEST);
            player->SetHP(GetTableManager()->GetCharacterStats()[(int)player->GetChacracterType()].hp);
            player->SetStamina(GetTableManager()->GetCharacterStats()[(int)player->GetChacracterType()].stamina);
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

    mPacketSender->SendPlayerAdd(roomID);
}

void Server::StartGame(int roomID)
{
    Room* room = GetRooms()[roomID];
    if (room->SetIsRun(true) == true) {

        mPacketSender->SendGameStart(roomID);

        // Push Event
        long long roomCode = room->GetRoomCode();
        GameMode gameMode = room->GetGameMode();
        GameModeInfo& modeInfo = mTableManager->GetGameModeData()[gameMode];

        PushEventBlockDrop(mTimer, roomID, roomCode, modeInfo.Block1_Spawn_Index, modeInfo.Block1_Spawn_Time);
        PushEventBlockDrop(mTimer, roomID, roomCode, modeInfo.Block2_Spawn_Index, modeInfo.Block2_Spawn_Time);

        PushEventBombSpawn(mTimer, roomID, roomCode, modeInfo.Bomb_Spawn_Time);

        PushEventWeaponSpawn(mTimer, roomID, roomCode, modeInfo.Weapon1_Spawn_Index, modeInfo.Weapon1_Spawn_Time);
        PushEventWeaponSpawn(mTimer, roomID, roomCode, modeInfo.Weapon2_Spawn_Index, modeInfo.Weapon2_Spawn_Time);

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
    std::set<int> winningTeams;
    for (auto iter = room->GetTeams().begin(); iter != room->GetTeams().end(); ++iter) {
        if (iter->second.GetLife() <= 0) {
            loseTeamCnt++;
        }
        else {
            winningTeams.insert(iter->first);
        }
    }

    if (loseTeamCnt == teamCnt - 1) {
        if (room->SetIsRun(false) == true) {
            mPacketSender->SendGameEndPacket(roomID, 0);

            CalculateGameResult(roomID, winningTeams);

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

    std::set<int> winningTeams;
    int maxLife = 0;

    for (auto iter = room->GetTeams().begin(); iter != room->GetTeams().end(); ++iter) {
        int life = iter->second.GetLife();
        if (maxLife < life) {
            winningTeams.clear();
            winningTeams.insert(iter->first);
            maxLife = life;
        }
        else if (maxLife == life) {
            winningTeams.insert(iter->first);
        }
    }

    if (room->SetIsRun(false) == true) {
        mPacketSender->SendGameEndPacket(roomID, 0);

        CalculateGameResult(roomID, winningTeams);

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

int Server::CalculatePoint(GameMode mode, sPlayerGameRecord record, bool isWin)
{
    int point = 0;

    auto& constants = mTableManager->GetPointConstantList()[mode];

    switch (isWin) {
    case true: {
        point = (record.kill_count * constants.Win_Kill_Point.Value) - (record.death_count * constants.Win_Death_Point.Value) + (record.bomb_insert_count * constants.Win_Bomb_Point.Value);
    }
             break;
    case false: {
        point = (record.kill_count * constants.Lose_Kill_Point.Value) - (record.death_count * constants.Lose_Death_Point.Value) + (record.bomb_insert_count * constants.Lose_Bomb_Point.Value);
    }
              break;
    default:
        break;
    }

    return (point < 0 ) ? 0 : point;
}

int Server::CalculateGoldReward(GameMode mode, int point, bool isMvp, bool isWin)
{
    int pointIdx = (point > 10) ? 10 : point;

    auto& rewards = mTableManager->GetGameRewardList()[mode];
    auto& BonusRewards = mTableManager->GetGameBonusRewardList()[mode][(isMvp == true) ? 11 : pointIdx];

    int gold = 0;

    switch (isWin) {
    case true:{
        gold += rewards.Win_Reward1_Value;
        if (isMvp) {
            gold += BonusRewards.MVP_Reward1_Value;
        }
        else {
            gold += BonusRewards.Win_Reward1_Value;
        }
    }
    break;

    case false: {
        gold += rewards.Lose_Reward1_Value + BonusRewards.Lose_Reward1_Value;
    }
    break;

    default:
        break;
    }

    return gold;
}

void Server::CalculateGameResult(int roomID, std::set<int>& winningTeams)
{
    Room* room = GetRooms()[roomID];
    std::unordered_map<int, sPlayerGameRecord>& records = room->GetPlayerRecordList();

    GameMode mode = room->GetGameMode();

    int mvp_id = INVALIDKEY;
    int mvp_point = -1;

    // 해당 게임 기록으로 결과 계산
    for (auto& pair : records) {
        int id = pair.first;

        int team = pair.second.team;
        bool isWin = (winningTeams.find(team) != winningTeams.end()) ? true : false;
        int point = CalculatePoint(mode, pair.second, isWin);
        pair.second.point = point;
        pair.second.earn_gold = CalculateGoldReward(mode, point, false, isWin);

        if (point > mvp_point) {
            mvp_id = id;
            mvp_point = point;
        }
    }

    if (mvp_id !=INVALIDKEY) {
        records[mvp_id].earn_gold = CalculateGoldReward(mode, records[mvp_id].point, true, true);
        records[mvp_id].is_mvp = true;
    }

    // DB에 데이터 업데이트
    room->GetPlayerListLock().lock_shared();
    for (auto& pair : records) {
        if (room->GetPlayerList()[pair.first] == nullptr) {
            continue;
        }
        sPlayerGameRecord record = pair.second;

        // 테스트용
        mDB->UpdateRanking(pair.first + 1001, record.kill_count, record.death_count, record.point);
        mDB->UpdateUserGold(pair.first + 1001, record.earn_gold);
        mDB->UpdateUserPoint(pair.first + 1001, record.point);
    }
    room->GetPlayerListLock().unlock_shared();

    mPacketSender->SendGameResultPacket(roomID, winningTeams);
}
