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

bool FITH::CheckGameEnd(int roomID)
{
    Room* room = pServer->GetRooms()[roomID];
    int teamCnt = pTableManager->GetGameModeData().at(mGameMode).Team_Count; // 팀 수
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

    // 한명만 남았을 경우
    if (loseTeamCnt == teamCnt - 1) {
        if (room->SetIsRun(false) == true) {
            pPacketSender->SendGameEndPacket(roomID, 0);

            CalculateGameResult(roomID, winningTeams);

            for (const auto& sID : room->GetPlayerList()) {
                int session_id = sID.load();
                if (session_id == INVALIDKEY) continue;

                Player* player = dynamic_cast<Player*>(pServer->GetSessions()[session_id]);

                player->GetSessionStateLock().lock();
                if (player->GetIsBot() == true) {
                    player->IngameInfoInit();
                    player->SetIsBot(false);
                    player->SetSessionState(eSessionState::ST_FREE);
                }
                else {
                    player->IngameInfoInit();
                    player->SetSessionState(eSessionState::ST_ACCEPTED);
                }
                player->GetSessionStateLock().unlock();
            }
            room->Reset();

            std::cout << "Game End - " << roomID << std::endl;
            return true;
        }
    }
    return false;
}

bool FITH::TimeoverGameEnd(int roomID)
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
            if (player->GetIsBot() == true) {
                player->IngameInfoInit();
                player->SetSessionState(eSessionState::ST_FREE);
            }
            else {
                player->IngameInfoInit();
                player->SetSessionState(eSessionState::ST_ACCEPTED);
            }
            player->GetSessionStateLock().unlock();
        }
        room->Reset();

        std::cout << "Game End - " << roomID << std::endl;
        return true;
    }

    return false;
}

int FITH::CalculatePoint(sPlayerGameRecord& record, BattleResult result)
{
    int point = 0;

    auto& constants = pTableManager->GetPointConstantList()[mGameMode];

    switch (result) {
    case BattleResult::BR_Win: {
        point = (record.gameRecord.KillCount.load() * constants.Win_Kill_Point.Value) - (record.gameRecord.DeathCount.load() * constants.Win_Death_Point.Value) + (record.gameRecord.Bomb_Count.load() * constants.Win_Bomb_Point.Value);
    }
             break;
    case BattleResult::BR_Lose: {
        point = (record.gameRecord.KillCount.load() * constants.Lose_Kill_Point.Value) - (record.gameRecord.DeathCount.load() * constants.Lose_Death_Point.Value) + (record.gameRecord.Bomb_Count.load() * constants.Lose_Bomb_Point.Value);
    }
              break;
    case BattleResult::BR_Draw: {
        point = (record.gameRecord.KillCount.load() * constants.Draw_Kill_Point.Value) - (record.gameRecord.DeathCount.load() * constants.Draw_Death_Point.Value) + (record.gameRecord.Bomb_Count.load() * constants.Draw_Bomb_Point.Value);
    }
                              break;
    default:
        break;
    }

    return (point < 0) ? 0 : point;
}

int FITH::CalculateGoldReward(int point, bool isMvp, BattleResult result)
{
    int pointIdx = (point > 10) ? 10 : point;

    auto& rewards = pTableManager->GetGameRewardList()[mGameMode];
    auto& BonusRewards = pTableManager->GetGameBonusRewardList()[mGameMode][(isMvp == true) ? 11 : pointIdx];

    int gold = 0;

    switch (result) {
    case BattleResult::BR_Win: {
        gold += rewards.Win_Reward1_Value;
        if (isMvp) {
            gold += BonusRewards.MVP_Reward1_Value;
        }
        else {
            gold += BonusRewards.Win_Reward1_Value;
        }
    }
             break;

    case BattleResult::BR_Lose: {
        gold += rewards.Lose_Reward1_Value + BonusRewards.Lose_Reward1_Value;
    }
              break;
    case BattleResult::BR_Draw: {
        gold += rewards.Draw_Reward1_Value + BonusRewards.Draw_Reward1_Value;
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

    BattleResult winFlag = BattleResult::BR_Win;

    if (winningTeams.size() > 1) {
        winFlag = BattleResult::BR_Draw;
    }


    // 해당 게임 기록으로 결과 계산
    for (auto& pair : records) {
        int id = pair.first;

        UserGameRecords& userGameRecord = pair.second.gameRecord;

        int team = pair.second.team;
        BattleResult result = (winningTeams.find(team) != winningTeams.end()) ? winFlag : BattleResult::BR_Lose;

        if (result == BattleResult::BR_Win) {
            userGameRecord.Victory_Count.store(1);
        }

        if (GameMode::FITH_Indiv_Battle_2 <= mGameMode && mGameMode <= GameMode::FITH_Indiv_Battle_5) {
            userGameRecord.FITH_Indiv_Count.store(1);
        }
        else if (GameMode::FITH_Team_Battle_4 <= mGameMode && mGameMode <= GameMode::FITH_Team_Battle_6){
            userGameRecord.FITH_Team_Count.store(1);
        }

        int point = CalculatePoint(pair.second, result);
        userGameRecord.Point = point;
        pair.second.earn_gold = CalculateGoldReward(point, false, result);

        if (point > mvp_point) {
            mvp_id = id;
            mvp_point = point;
        }

    }

    if (winFlag == BattleResult::BR_Win) {
        if (mvp_id != INVALIDKEY) {
            records[mvp_id].earn_gold = CalculateGoldReward(records[mvp_id].gameRecord.Point, true, BattleResult::BR_Win);
            records[mvp_id].is_mvp = true;
        }
    }


    // DB에 데이터 업데이트
    for (auto& pair : records) {
        sPlayerGameRecord& record = pair.second;

        int sessionID = room->GetPlayerList()[pair.first].load();

        if (sessionID == INVALIDKEY) {
            continue;
        }

        Player* player = dynamic_cast<Player*>(pServer->GetSessions()[sessionID]);

        int uid = player->GetUID();

        if (uid == INVALIDKEY) {
            continue;
        }

        pDB->UpdateBattleRecords(uid, record.gameRecord);
        pDB->UpsertUserItemCount(uid, 100001, record.earn_gold);
        pDB->UpdateUserPoint(uid, record.gameRecord.Point);
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
        long long uniqueCode = room->GetBomb(id)->GetUniqueCode();
        PushEventBombExplosion(pServer->GetTimer(), roomID, id, uniqueCode, room->GetRoomCode(), explosionInterval);
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
