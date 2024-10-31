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
    pServer = server;
    pDB = pServer->GetDB();
    pTimer = pServer->GetTimer();
    pTableManager = pServer->GetTableManager();
    pPacketSender = pServer->GetPacketSender();

    mGameMode = gameMode;
}

void FITH::StartGame(int roomID)
{
    Room* room = pServer->GetRooms()[roomID];

    if (pServer->GetMode() == SERVER_MODE::TEST) {
       
    }

    if (room->SetIsRun(true) == true) {

        pPacketSender->SendGameStart(roomID);

        // Push Event
        long long roomCode = room->GetRoomCode();
        GameModeData& modeInfo = room->GetGameModeData();

        PushEventBlockDrop(pTimer, roomID, roomCode, modeInfo.Block1_Spawn_Index, modeInfo.Block1_Spawn_Time);
        PushEventBlockDrop(pTimer, roomID, roomCode, modeInfo.Block2_Spawn_Index, modeInfo.Block2_Spawn_Time);

        PushEventBombSpawn(pTimer, roomID, roomCode, modeInfo.Bomb_Spawn_Time);

        PushEventWeaponSpawn(pTimer, roomID, roomCode, modeInfo.Weapon1_Spawn_Index, modeInfo.Weapon1_Spawn_Time);
        PushEventWeaponSpawn(pTimer, roomID, roomCode, modeInfo.Weapon2_Spawn_Index, modeInfo.Weapon2_Spawn_Time);

        PushEventRemainTimeSync(pTimer, roomID, roomCode);

        PushEventTimeOverCheck(pTimer, roomID, roomCode);

        room->SetStartTime(std::chrono::system_clock::now());
    }
}

void FITH::CheckGameEnd(int roomID)
{
    Room* room = pServer->GetRooms()[roomID];
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
            pPacketSender->SendGameEndPacket(roomID, 0);

            CalculateGameResult(roomID, winningTeams);

            for (const auto& sID : room->GetPlayerList()) {
                int session_id = sID.load();
                if (session_id == INVALIDKEY) continue;

                Player* player = dynamic_cast<Player*>(pServer->GetSessions()[session_id]);

                player->GetSessionStateLock().lock();
                player->SetSessionState(eSessionState::ST_ACCEPTED);
                player->GetSessionStateLock().unlock();
            }
            room->Reset();

            std::cout << "Game End - " << roomID << std::endl;
        }
    }
}

void FITH::TimeoverGameEnd(int roomID)
{
    Room* room = pServer->GetRooms()[roomID];

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
        pPacketSender->SendGameEndPacket(roomID, 0);

        CalculateGameResult(roomID, winningTeams);

        for (const auto& sID : room->GetPlayerList()) {
            int session_id = sID.load();
            if (session_id == INVALIDKEY) continue;
            Player* player = dynamic_cast<Player*>(pServer->GetSessions()[session_id]);
            player->GetSessionStateLock().lock();
            player->Init();
            player->SetSessionState(eSessionState::ST_ACCEPTED);
            player->GetSessionStateLock().unlock();
        }
        room->Reset();

        std::cout << "Game End - " << roomID << std::endl;
    }
}

int FITH::CalculatePoint(sPlayerGameRecord record, bool isWin)
{
    int point = 0;

    auto& constants = pTableManager->GetPointConstantList()[mGameMode];

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

    auto& rewards = pTableManager->GetGameRewardList()[mGameMode];
    auto& BonusRewards = pTableManager->GetGameBonusRewardList()[mGameMode][(isMvp == true) ? 11 : pointIdx];

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
    Room* room = pServer->GetRooms()[roomID];
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
    for (auto& pair : records) {
        sPlayerGameRecord record = pair.second;

        //// 테스트용
        //pDB->UpdateRanking(pair.first + 1001, record.kill_count, record.death_count, record.point);
        //pDB->UpdateUserGold(pair.first + 1001, record.earn_gold);
        //pDB->UpdateUserPoint(pair.first + 1001, record.point);
    }

    pPacketSender->SendGameResultPacket(roomID, winningTeams);
}

bool FITH::DeletePlayer(int playerID, int roomID)
{
    Room* room = pServer->GetRooms()[roomID];

    int sessionID = room->GetPlayerList()[playerID].load();
    if (sessionID == INVALIDKEY) {
        return false;
    }

    Player* player = dynamic_cast<Player*>(pServer->GetSessions()[sessionID]);

    if (room->GetState() != eRoomState::RS_INGAME) {
        return false;
    }

    if (player == nullptr) {
        return false;
    }
    Vector3f pos = player->GetPosition();
    Vector3f dir = player->GetDirection();

    // 들고있는 무기 해제
    player->GetWeaponLock().lock();
    Weapon* weapon = player->GetWeapon();
    if (weapon != nullptr) {
        if (weapon->SetIsGrabbed(false) == true) {
            int weaponID = weapon->GetID();
            weapon->SetOwenrID(INVALIDKEY);
            weapon->SetPosition(player->GetPosition());
            pPacketSender->SendWeaponDropPacket(pos, roomID, weaponID);
        }
    }
    player->GetWeaponLock().unlock();

    // 들고있는 폭탄 해제
    player->GetBombLock().lock();
    Bomb* bomb = player->GetBomb();
    if (bomb != nullptr) {
        if (bomb->SetIsGrabbed(false) == true) {
            int bombID = bomb->GetID();
            bomb = nullptr;
            pPacketSender->SendBombDropPacket(pos, roomID, bombID);
        }
    }
    player->GetBombLock().unlock();

    // 잡은 플레이어 놓기
    if (player->GetAttachedPlayerID() != INVALIDKEY && player->GetIsGrabbed() == false) {
        int targetID = player->GetAttachedPlayerID();
        Player* target = dynamic_cast<Player*>(pServer->GetSessions()[room->GetPlayerList()[targetID].load()]);

        if (target->SetIsGrabbed(false) == true) {
            player->SetAttachedPlayerID(INVALIDKEY);
            target->SetAttachedPlayerID(INVALIDKEY);
            pPacketSender->SendPlayerThrowOtherPlayerPacket(roomID, playerID, pos, dir, targetID, target->GetPosition(), target->GetDirection());
        }
    }

    room->GetTeams()[player->GetTeam()].GetMembers().erase(playerID);
    room->GetPlayerList()[playerID].store(INVALIDKEY);
    if (room->GetPlayerCnt() > 0) {
        room->SubPlayerCnt();
    }
    return true;
}

std::set<Vector3f> FITH::SetObjectSpawnPos(int roomID, int spawnCount)
{
    Room* room = pServer->GetRooms()[roomID];

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
    GameModeData& modeInfo = room->GetGameModeData();

    int explosionInterval = modeInfo.Bomb_Delay_Time;
    int spawnCount = modeInfo.Bomb_Spawn_Count;

    std::set<Vector3f> spawnPoses = this->SetObjectSpawnPos(roomID, spawnCount);

    float posOffset = (static_cast<float>(BLOCKSIZE) / 3);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<>dis_value(-posOffset, posOffset);

    std::vector<Vector3f> poses;
    std::vector<int> bombIDs;

    for (auto& pos : spawnPoses) {
        Vector3f newPos;
        newPos.x = pos.x + (float)dis_value(gen);
        newPos.z = pos.z + (float)dis_value(gen);

        int bombid = room->AddBomb(newPos);

        if (bombid == INVALIDKEY) continue;
        poses.push_back(newPos);
        bombIDs.push_back(bombid);
    }

    pPacketSender->SendBombSpawnPacket(poses, bombIDs, explosionInterval, roomID);

    for (const int id : bombIDs) {
        PushEventBombExplosion(pServer->GetTimer(), roomID, id, room->GetRoomCode(), explosionInterval);
    }
}

void FITH::WeaponSpawn(Room* room, int roomID, eWeaponType weaponType, int spawnCount)
{
    std::set<Vector3f> spawnPoses = this->SetObjectSpawnPos(roomID, spawnCount);

    std::vector<Vector3f> poses;
    std::vector<int> weaponIDs;
    std::vector<int> weaponTypes;
    WeaponStat& weaponStat = pServer->GetTableManager()->GetWeaponStats()[weaponType];

    float posOffset = (static_cast<float>(BLOCKSIZE) / 3);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<>dis_value(-posOffset, posOffset);

    for (const auto& pos : spawnPoses) {
        Vector3f newPos;
        newPos.x = pos.x + (float)dis_value(gen);
        newPos.z = pos.z + (float)dis_value(gen);

        int weaponid = room->AddWeapon(weaponType, weaponStat, newPos);
        if (weaponid == INVALIDKEY) continue;

        poses.push_back(newPos);
        weaponIDs.push_back(weaponid);
        weaponTypes.push_back(weaponType);
    }
    pPacketSender->SendWeaponSpawnPacket(poses, weaponIDs, weaponTypes, roomID);
}

bool FITH::CheckValidPlayerPosition(Vector3f& position)
{
    if (position.y <= -15) {
        return false;
    }
    return true;
}
