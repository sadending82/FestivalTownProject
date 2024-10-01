#pragma once
#include "FITH.h"
#include "../../DB/DB.h"
#include "../../Server/Server.h"
#include "../../Room/Room.h"
#include "../../PacketManager/PacketSender/PacketSender.h"
#include "../../Event/Event.h"
#include <random>

FITH::FITH(class Server* server, GameMode gameMode)
{
    mServer = server;
    mDB = mServer->GetDB();
    mTimer = mServer->GetTimer();
    mTableManager = mServer->GetTableManager();
    mPacketSender = mServer->GetPacketSender();

    mGameMode = gameMode;
}

void FITH::StartGame(int roomID)
{
    Room* room = mServer->GetRooms()[roomID];

    if (mServer->GetMode() == SERVER_MODE::TEST) {
        mPacketSender->SendGameStart(roomID);
    }

    if (room->SetIsRun(true) == true) {

        if (mServer->GetMode() == SERVER_MODE::TEST) {
            return;
        }

        mPacketSender->SendGameStart(roomID);

        // Push Event
        long long roomCode = room->GetRoomCode();
        GameModeInfo& modeInfo = mTableManager->GetGameModeData()[mGameMode];

        PushEventBlockDrop(mTimer, roomID, roomCode, modeInfo.Block1_Spawn_Index, modeInfo.Block1_Spawn_Time);
        PushEventBlockDrop(mTimer, roomID, roomCode, modeInfo.Block2_Spawn_Index, modeInfo.Block2_Spawn_Time);

        PushEventBombSpawn(mTimer, roomID, roomCode, modeInfo.Bomb_Spawn_Time);

        PushEventWeaponSpawn(mTimer, roomID, roomCode, modeInfo.Weapon1_Spawn_Index, modeInfo.Weapon1_Spawn_Time);
        PushEventWeaponSpawn(mTimer, roomID, roomCode, modeInfo.Weapon2_Spawn_Index, modeInfo.Weapon2_Spawn_Time);

        PushEventRemainTimeSync(mTimer, roomID, roomCode);

        PushEventTimeOverCheck(mTimer, roomID, roomCode);

        room->SetStartTime(std::chrono::system_clock::now());
    }
}

void FITH::CheckGameEnd(int roomID)
{
    Room* room = mServer->GetRooms()[roomID];
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
            room->Reset();

            std::cout << "Game End - " << roomID << std::endl;
        }
    }
}

void FITH::TimeoverGameEnd(int roomID)
{
    Room* room = mServer->GetRooms()[roomID];

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
        room->Reset();

        std::cout << "Game End - " << roomID << std::endl;
    }
}

int FITH::CalculatePoint(sPlayerGameRecord record, bool isWin)
{
    int point = 0;

    auto& constants = mTableManager->GetPointConstantList()[mGameMode];

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

    return (point < 0) ? 0 : point;
}

int FITH::CalculateGoldReward(int point, bool isMvp, bool isWin)
{
    int pointIdx = (point > 10) ? 10 : point;

    auto& rewards = mTableManager->GetGameRewardList()[mGameMode];
    auto& BonusRewards = mTableManager->GetGameBonusRewardList()[mGameMode][(isMvp == true) ? 11 : pointIdx];

    int gold = 0;

    switch (isWin) {
    case true: {
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

void FITH::CalculateGameResult(int roomID, std::set<int>& winningTeams)
{
    Room* room = mServer->GetRooms()[roomID];
    std::unordered_map<int, sPlayerGameRecord>& records = room->GetPlayerRecordList();

    int mvp_id = INVALIDKEY;
    int mvp_point = -1;

    // 해당 게임 기록으로 결과 계산
    for (auto& pair : records) {
        int id = pair.first;

        int team = pair.second.team;
        bool isWin = (winningTeams.find(team) != winningTeams.end()) ? true : false;
        int point = CalculatePoint(pair.second, isWin);
        pair.second.point = point;
        pair.second.earn_gold = CalculateGoldReward(point, false, isWin);

        if (point > mvp_point) {
            mvp_id = id;
            mvp_point = point;
        }
    }

    if (mvp_id != INVALIDKEY) {
        records[mvp_id].earn_gold = CalculateGoldReward(records[mvp_id].point, true, true);
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

std::set<Vector3f> FITH::SetObjectSpawnPos(int roomID, int spawnCount)
{
    Room* room = mServer->GetRooms()[roomID];

    std::vector<std::pair<int, int>>& spawnPoses = room->GetMap()->GetObjectSpawnIndexes();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<>idx_distrib(0, spawnPoses.size() - 1);

    std::set<Vector3f> unique_pos;
    std::set<int> invalid_pos_index;

    while (unique_pos.size() < spawnCount) {
        int idx = idx_distrib(gen);
        Vector3f spawnIndex = Vector3f(spawnPoses[idx].first, 0, spawnPoses[idx].second);
        bool invalid_pos = false;

        if (invalid_pos == false) {
            unique_pos.emplace(ConvertVec2iToVec3f(spawnIndex.x, spawnIndex.z));
        }
    }

    return unique_pos;
}

void FITH::BombSpawn(Room* room, int roomID)
{
    GameModeInfo& modeInfo = mServer->GetTableManager()->GetGameModeData()[mGameMode];

    int explosionInterval = modeInfo.Bomb_Delay_Time;
    int spawnCount = modeInfo.Bomb_Spawn_Count;

    std::set<Vector3f> spawnPoses = this->SetObjectSpawnPos(roomID, spawnCount);

    for (const auto& pos : spawnPoses) {
        int bombid = room->AddBomb(new Bomb, pos);
        if (bombid == INVALIDKEY) continue;
        mPacketSender->SendBombSpawnPacket(pos, bombid, roomID);
        PushEventBombExplosion(mServer->GetTimer(), roomID, bombid, room->GetRoomCode(), explosionInterval);
    }
}

void FITH::WeaponSpawn(Room* room, int roomID, eWeaponType weaponType, int spawnCount)
{
    std::set<Vector3f> spawnPoses = this->SetObjectSpawnPos(roomID, spawnCount);

    for (const auto& pos : spawnPoses) {
        int weaponid = room->AddWeapon(new Weapon(weaponType, nullptr), pos);
        if (weaponid == INVALIDKEY) continue;
        mPacketSender->SendWeaponSpawnPacket(pos, weaponid, roomID, weaponType);
    }
}

bool FITH::CheckValidPlayerPosition(Vector3f& position)
{
    if (position.y <= -15) {
        return false;
    }
    return true;
}
