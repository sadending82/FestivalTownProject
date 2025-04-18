#pragma once
#include "FITH.h"
#include "../../DB/DB.h"
#include "../../Server/Server.h"
#include "../../Room/Room.h"
#include "../../PacketManager/PacketSender/PacketSender.h"
#include "../../Event/Event.h"
#include <chrono>
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

void FITH::PrepareForStartGame(Room* room, int roomID)
{
    PushEventGameStart(pServer->GetTimer(), roomID, room->GetRoomCode());
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
        PushEventBlockDrop(pTimer, roomID, roomCode, modeInfo.Block2_Spawn_Index, (float)modeInfo.Block2_Spawn_Time + SECONDBLOCKSPAWNADDINTERVAL);

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
    int teamCnt = pTableManager->GetGameModeOutData().at(mGameMode).Team_Count; // 팀 수
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

                if (player->GetIsBot() == true) {
                    player->IngameInfoInit();
                    player->SetIsBot(false);
                    player->SetSessionState(eSessionState::ST_FREE);
                }
                else {
                    player->IngameInfoInit();
                    player->SetSessionState(eSessionState::ST_ACCEPTED);
                }
            }
            room->Reset();

            //std::cout << "Game End - " << roomID << std::endl;
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
            if (player->GetIsBot() == true) {
                player->IngameInfoInit();
                player->SetSessionState(eSessionState::ST_FREE);
            }
            else {
                player->IngameInfoInit();
                player->SetSessionState(eSessionState::ST_ACCEPTED);
            }
        }

        room->Reset();

        //std::cout << "Game End - " << roomID << std::endl;
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

std::vector<sGameReward> FITH::CalculateGameReward(int point, bool isMvp, BattleResult result)
{
    int pointIdx = (point > 10) ? 10 : point;

    auto& rewards = pTableManager->GetGameRewardList()[mGameMode];
    auto& BonusRewards = pTableManager->GetGameBonusRewardList()[mGameMode][(isMvp == true) ? 11 : pointIdx];

    int gold = 0;

    sGameReward reward1;
    sGameReward reward2;
    sGameReward reward3;

    switch (result) {
    case BattleResult::BR_Win: {
        reward1.index = (rewards.Win_Reward1_Index != 0) ? rewards.Win_Reward1_Index : BonusRewards.Win_Reward1_Index;
        reward1.value += rewards.Win_Reward1_Value;
        if (isMvp) {
            reward1.index = (rewards.Win_Reward1_Index != 0) ? rewards.Win_Reward1_Index : BonusRewards.MVP_Reward1_Index;
            reward1.value += BonusRewards.MVP_Reward1_Value;
        }
        else {
            reward1.value += BonusRewards.Win_Reward1_Value;
        }

        reward2.index = (rewards.Win_Reward2_Index != 0) ? rewards.Win_Reward2_Index : BonusRewards.Win_Reward2_Index;
        reward2.value += rewards.Win_Reward2_Value;
        if (isMvp) {
            reward2.index = (reward2.index != 0) ? reward2.index : BonusRewards.MVP_Reward2_Index;
            reward2.value += BonusRewards.MVP_Reward2_Value;
        }
        else {
            reward2.value += BonusRewards.Win_Reward2_Value;
        }

        reward3.index = (rewards.Win_Reward3_Index != 0) ? rewards.Win_Reward3_Index : BonusRewards.Win_Reward3_Index;
        reward3.value += rewards.Win_Reward3_Value;
        if (isMvp) {
            reward3.index = (reward3.index != 0) ? reward3.index : BonusRewards.MVP_Reward3_Index;
            reward3.value += BonusRewards.MVP_Reward3_Value;
        }
        else {
            reward3.value += BonusRewards.Win_Reward3_Value;
        }
    }
             break;

    case BattleResult::BR_Lose: {
        reward1.index = (rewards.Lose_Reward1_Index != 0) ? rewards.Lose_Reward1_Index : BonusRewards.Lose_Reward1_Index;
        if (reward1.index != 0) reward1.value += rewards.Lose_Reward1_Value + BonusRewards.Lose_Reward1_Value;
        
        reward2.index = (rewards.Lose_Reward2_Index != 0) ? rewards.Lose_Reward2_Index : BonusRewards.Lose_Reward2_Index;
        if (reward2.index != 0) reward2.value += rewards.Lose_Reward2_Value + BonusRewards.Lose_Reward2_Value;

        reward3.index = (rewards.Lose_Reward3_Index != 0) ? rewards.Lose_Reward3_Index : BonusRewards.Lose_Reward3_Index;
        if (reward3.index != 0) reward3.value += rewards.Lose_Reward3_Value + BonusRewards.Lose_Reward3_Value;
    }
              break;
    case BattleResult::BR_Draw: {
        reward1.index = (rewards.Draw_Reward1_Index != 0) ? rewards.Draw_Reward1_Index : BonusRewards.Draw_Reward1_Index;
        if (reward1.index != 0) reward1.value += rewards.Draw_Reward1_Value + BonusRewards.Draw_Reward1_Value;

        reward2.index = (rewards.Draw_Reward2_Index != 0) ? rewards.Draw_Reward2_Index : BonusRewards.Draw_Reward2_Index;
        if (reward2.index != 0) reward2.value += rewards.Draw_Reward2_Value + BonusRewards.Draw_Reward2_Value;

        reward3.index = (rewards.Draw_Reward3_Index != 0) ? rewards.Draw_Reward3_Index : BonusRewards.Draw_Reward3_Index;
        if (reward3.index != 0) reward3.value += rewards.Draw_Reward3_Value + BonusRewards.Draw_Reward3_Value;
    }
                              break;
    default:
        break;
    }

    return std::vector<sGameReward>{reward1, reward2, reward3};
}

void FITH::CalculateGameResult(int roomID, std::set<int>& winningTeams)
{
    Room* room = pServer->GetRooms()[roomID];

    std::unordered_map<int, sPlayerGameRecord>& records = room->GetPlayerRecordList();

    int mvp_id = INVALIDKEY;
    int mvp_point = -1;

    TIMEPOINT endTime = std::chrono::system_clock::now();

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

        pair.second.battleResult = result;

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
        pair.second.rewards = CalculateGameReward(point, false, result);

        if (point > mvp_point) {
            mvp_id = id;
            mvp_point = point;
        }

        userGameRecord.Play_Time_Count = (int)std::chrono::duration_cast<std::chrono::seconds>(endTime - room->GetStartTime()).count();
    }

    if (winFlag == BattleResult::BR_Win) {
        if (mvp_id != INVALIDKEY) {
            records[mvp_id].rewards = CalculateGameReward(records[mvp_id].gameRecord.Point, true, BattleResult::BR_Win);
            records[mvp_id].is_mvp = true;
        }
    }


    // DB에 데이터 업데이트
    if (room->GetGameMode() != GameMode::FITH_Tutorial_1) {
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

            std::vector<UserItem> rewardList;
            for (int i = 0; i < record.rewards.size(); ++i) {
                if (record.rewards[i].index == 0) {
                    continue;
                }

                const int itemIndex = record.rewards[i].index;
                const int itemAmount = record.rewards[i].value;
                rewardList.push_back(UserItem(uid, itemIndex, itemAmount));
                pDB->UpsertUserCurrencyRecord(uid, itemIndex, itemAmount, 0);

                const int itemType = (int)pTableManager->GetItemInfos()[itemIndex].Item_Type;

                player->GetItems()[record.rewards[i].index].itemCode = itemIndex;
                player->GetItems()[record.rewards[i].index].count += itemAmount;
                player->GetItems()[record.rewards[i].index].itemType = itemType;
            }
            pDB->UpsertUserCurrency(uid, rewardList);

            UpdateMissionbyGameRecord(player, record);
        }
    }

    pPacketSender->SendGameResultPacket(roomID, winningTeams);
}

void FITH::UpdateMissionbyGameRecord(Player* player, sPlayerGameRecord& gameRecord)
{
    // 임시
    int pass_index = 601;

    std::vector<UserMission> updatedMissionList;

    int killCount = gameRecord.gameRecord.KillCount;
    int groggyCount = gameRecord.gameRecord.Groggy_Count;
    int inputBombCount = gameRecord.gameRecord.Bomb_Count;

    // 킬 미션
    if (killCount != 0) {
        // 일일 미션
        for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_DAILY][eMissionCategory::MC_KILL]) {

            std::unordered_map<int, UserMission>& missionInfos = missionGroupList.second;

            for (auto& missionInfo : missionInfos) {
                missionInfo.second.progress += killCount;

                updatedMissionList.push_back(missionInfo.second);
            }
        }

        // 패스 미션
        for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_PASS][eMissionCategory::MC_KILL]) {

            std::unordered_map<int, UserMission>& missionInfos = missionGroupList.second;

            for (auto& missionInfo : missionInfos) {
                missionInfo.second.progress += killCount;

                updatedMissionList.push_back(missionInfo.second);
            }
        }
    }

    // 폭탄 투입 미션
    if (inputBombCount != 0) {
        // 일일 미션
        for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_DAILY][eMissionCategory::MC_INPUT_BOMB]) {

            std::unordered_map<int, UserMission>& missionInfos = missionGroupList.second;

            for (auto& missionInfo : missionInfos) {
                missionInfo.second.progress += inputBombCount;
                updatedMissionList.push_back(missionInfo.second);
            }
        }

        // 패스 미션
        for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_PASS][eMissionCategory::MC_INPUT_BOMB]) {

            std::unordered_map<int, UserMission>& missionInfos = missionGroupList.second;

            for (auto& missionInfo : missionInfos) {
                missionInfo.second.progress += inputBombCount;
                updatedMissionList.push_back(missionInfo.second);
            }
        }
    }

    // 그로기 미션
    if (groggyCount != 0) {
        // 일일 미션
        for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_DAILY][eMissionCategory::MC_GROGGY]) {

            std::unordered_map<int, UserMission>& missionInfos = missionGroupList.second;

            for (auto& missionInfo : missionInfos) {
                missionInfo.second.progress += groggyCount;
                updatedMissionList.push_back(missionInfo.second);
            }
        }

        // 패스 미션
        for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_PASS][eMissionCategory::MC_GROGGY]) {

            std::unordered_map<int, UserMission>& missionInfos = missionGroupList.second;

            for (auto& missionInfo : missionInfos) {
                missionInfo.second.progress += groggyCount;
                updatedMissionList.push_back(missionInfo.second);
            }
        }
    }

    // 승리 미션
    if (gameRecord.battleResult == BattleResult::BR_Win) {
        // 일일 미션
        for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_DAILY][eMissionCategory::MC_GAME_WINNING]) {

            std::unordered_map<int, UserMission>& missionInfos = missionGroupList.second;

            for (auto& missionInfo : missionInfos) {
                missionInfo.second.progress++;
                updatedMissionList.push_back(missionInfo.second);
            }
        }

        // 패스 미션
        for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_PASS][eMissionCategory::MC_GAME_WINNING]) {

            std::unordered_map<int, UserMission>& missionInfos = missionGroupList.second;

            for (auto& missionInfo : missionInfos) {
                missionInfo.second.progress++;
                updatedMissionList.push_back(missionInfo.second);
            }
        }
    }

    // 플레이 미션
    {
        for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_DAILY][eMissionCategory::MC_GAME_PLAY]) {

            std::unordered_map<int, UserMission>& missionInfos = missionGroupList.second;

            for (auto& missionInfo : missionInfos) {
                missionInfo.second.progress++;
                updatedMissionList.push_back(missionInfo.second);
            }
        }

        // 패스 미션
        for (auto& missionGroupList : player->GetMissionList().missionList[pass_index][eMissionType::MT_PASS][eMissionCategory::MC_GAME_PLAY]) {

            std::unordered_map<int, UserMission>& missionInfos = missionGroupList.second;

            for (auto& missionInfo : missionInfos) {
                missionInfo.second.progress++;
                updatedMissionList.push_back(missionInfo.second);
            }
        }
    }

    if (!updatedMissionList.empty()) {
        pDB->UpsertUserMission(player->GetUID(), updatedMissionList);
    }
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

    /*if (room->GetPlayerCnt() <= 0) {
        room->Reset();
    }*/

    return true;
}

std::set<Vector3f> FITH::SetObjectSpawnPos(int roomID, int spawnCount)
{
    Room* room = pServer->GetRooms()[roomID];

    std::vector<std::pair<int, int>>& spawnPoses = room->GetMap().GetObjectSpawnIndexes();

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

    if (spawnPoses.empty()) return;

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
        Bomb* bomb = room->GetBomb(id);
        if (bomb == nullptr) {
            std::cout << "ERROR!: " << "GetBomb FAIL. Bomb ID: " << id << std::endl;;
            continue;
        }
        long long uniqueCode = bomb->GetUniqueCode();
        PushEventBombExplosion(pServer->GetTimer(), roomID, id, uniqueCode, room->GetRoomCode(), explosionInterval);
    }
}

void FITH::WeaponSpawn(Room* room, int roomID, eWeaponType weaponType, int spawnCount)
{
    std::set<Vector3f> spawnPoses = this->SetObjectSpawnPos(roomID, spawnCount);

    if (spawnPoses.empty()) return;

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

void FITH::PlayerSpawn(Room* room, int roomID, Player* player)
{
    int team = player->GetTeam();

    std::vector<std::pair<int, int>>& spawnPoses = pServer->GetRooms()[roomID]->GetMap().GetPlayerSpawnIndexes(team);

    Vector3f pos = Vector3f();
    bool canSpawn = false;

    while (1) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> idx_distrib(0, spawnPoses.size() - 1);
        int idx = idx_distrib(gen);
        int posX = spawnPoses[idx].first;
        int posY = spawnPoses[idx].second;

        for (int memberID : room->GetTeams()[team].GetMembers()) {
            Player* member = dynamic_cast<Player*>(pServer->GetSessions()[room->GetPlayerList()[memberID]]);
            Vector3f memberPos = ConvertVec3fToVec2i(member->GetPosition());

            if (memberPos.x == posX && memberPos.y == posY) {
                canSpawn = true;
                break;
            }
            else {
                canSpawn = true;
            }
        }

        if (canSpawn == true) {
            pos = ConvertVec2iToVec3f(posX, posY);
            break;
        }
    }

    pPacketSender->SendPlayerRespawn(player->GetInGameID(), roomID, pos);
}

bool FITH::PlayerDamageReceive(int roomID, Room* room, int attackerID, Player* attacker, int targetID, Player* target
    , Vector3f knockback_direction, eDamageType damageType)
{
    switch (damageType)
    {
    case eDamageType::AT_FALLDOWN: {
        return PlayerDieInstant(roomID, room, targetID, target);
    }break;
    case eDamageType::AT_BOMB_ATTACK: {
        return PlayerDamagedFromBomb(roomID, room, targetID, target, knockback_direction);
    }break;

    default: {
        return PlayerDamagedFromOther(roomID, room, attackerID, attacker, targetID, target, knockback_direction, damageType);
    }break;
    }
    return false;
}

bool FITH::PlayerDieInstant(int roomID, Room* room, int targetID, Player* target)
{
    sPlayerGameRecord& targetGameRecord = room->GetPlayerRecordList().at(targetID);

    int damageAmount = 9999999;
    int spawnTime = room->GetGameModeData().Player_Spawn_Time;
    if (target->ChangeToDeadState(pServer, spawnTime)) {
        // 사망 처리
        target->ReduceHP(damageAmount);
        // record update
        targetGameRecord.gameRecord.DeathCount.fetch_add(1);
        PushEventPlayerRespawn(pServer->GetTimer(), targetID, roomID, room->GetRoomCode(), spawnTime);
        return true;
    }

    return false;
}

bool FITH::PlayerDamagedFromOther(int roomID, Room* room, int attackerID, Player* attacker
    , int targetID, Player* target, Vector3f knockback_direction, eDamageType damageType)
{
    sPlayerGameRecord& attackerGameRecord = room->GetPlayerRecordList().at(attackerID);
    sPlayerGameRecord& targetGameRecord = room->GetPlayerRecordList().at(targetID);

    CharacterStat& attackerStat = attacker->GetCharacterStat();
    int damageAmount = 0;
    int TargetStaminaLoss = 0;

    int currStamina = target->GetStamina();

    if (targetID == attackerID) {
        return false;
    }
    // 팀킬 방지
    if (target->GetTeam() == attacker->GetTeam()) {
        return false;
    }

    // 살아있는지 확인
    if (target->GetPlayerState() != ePlayerState::PS_ALIVE) {
        return false;
    }

    // 데미지 계산
    damageAmount += attackerStat.strength * attackerStat.attackStats.at(damageType).Value;
    attacker->GetWeaponLock().lock_shared();
    if (attacker->GetWeapon() != nullptr) {
        damageAmount = attackerStat.strength + attacker->GetWeapon()->GetStat().Weapon_Power;
    }
    TargetStaminaLoss += attackerStat.attackStats.at(damageType).Vanish_Stamina;

    // 데미지 적용
    target->ReduceHP(damageAmount);
    currStamina -= TargetStaminaLoss;


    if (target->GetHP() <= 0) {
        // 사망 처리
        int spawnTime = room->GetGameModeData().Player_Spawn_Time;
        target->ChangeToDeadState(pServer, spawnTime);

        // record update
        attackerGameRecord.gameRecord.KillCount.fetch_add(1);
        targetGameRecord.gameRecord.DeathCount.fetch_add(1);

        if (attacker->GetWeapon() != nullptr) {
            attackerGameRecord.gameRecord.Weapon_Kill_Count.fetch_add(1);
        }
        else {
            attackerGameRecord.gameRecord.Punch_Kill_Count.fetch_add(1);
        }

        PushEventPlayerRespawn(pServer->GetTimer(), targetID, roomID, room->GetRoomCode(), spawnTime);
    }
    else {
        target->SetStamina(currStamina);
        pPacketSender->SendPlayerCalculatedDamage(targetID, roomID, eDamageType::AT_ATTACK, target->GetHP(), damageAmount, currStamina, knockback_direction);

        // 타격 후 스테미너가 0미만이면 그로기로
        if (currStamina < 0) {
            if (target->ChangeToGroggyState(pServer)) {
                attackerGameRecord.gameRecord.Groggy_Count.fetch_add(1);
                long long groggyTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                target->SetLastGroggyTime(groggyTime);
                PushEventGroggyRecovery(pServer->GetTimer(), targetID, roomID, groggyTime, room->GetRoomCode(), room->GetGameModeData().Ch_Groggy);
            }
        }
    }
    attacker->GetWeaponLock().unlock_shared();

    return true;
}

bool FITH::PlayerDamagedFromBomb(int roomID, Room* room, int targetID, Player* target, Vector3f knockback_direction)
{
    sPlayerGameRecord& targetGameRecord = room->GetPlayerRecordList().at(targetID);

    // 살아있는지 확인
    if (target->GetPlayerState() != ePlayerState::PS_ALIVE) {
        return false;
    }


    int damageAmount = room->GetGameModeData().Bomb_Damage;

    // 타겟의 기력이 없으면 그로기 상태로
    if (target->GetStamina() <= 0) {
        if (target->ChangeToGroggyState(pServer)) {
            long long groggyTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            target->SetLastGroggyTime(groggyTime);
            PushEventGroggyRecovery(pServer->GetTimer(), targetID, roomID, groggyTime, room->GetRoomCode(), room->GetGameModeData().Ch_Groggy);
        }
    }

    // 데미지 적용
    target->ReduceHP(damageAmount);

    if (target->GetHP() <= 0) {
        // 사망 처리
        int spawnTime = room->GetGameModeData().Player_Spawn_Time;
        target->ChangeToDeadState(pServer, spawnTime);

        // record update
        targetGameRecord.gameRecord.DeathCount.fetch_add(1);
        PushEventPlayerRespawn(pServer->GetTimer(), targetID, roomID, room->GetRoomCode(), spawnTime);
    }
    else {
        // 다른 플레이어 잡고있으면 풀기
        int attachedPlayerID = target->GetAttachedPlayerID();
        if (attachedPlayerID != INVALIDKEY && target->GetIsGrabbed() == false) {
            if (room->GetState() == eRoomState::RS_INGAME) {
                Player* attachedPlayer = dynamic_cast<Player*>(pServer->GetSessions()[room->GetPlayerList()[attachedPlayerID]]);

                if (attachedPlayer->SetIsGrabbed(false) == true) {
                    target->SetAttachedPlayerID(INVALIDKEY);
                    attachedPlayer->SetAttachedPlayerID(INVALIDKEY);
                    pServer->GetPacketSender()->SendPlayerThrowOtherPlayerPacket(roomID, targetID, target->GetPosition(), target->GetDirection()
                        , attachedPlayerID, attachedPlayer->GetPosition(), attachedPlayer->GetDirection());
                }
            }
        }

        pPacketSender->SendPlayerCalculatedDamage(targetID, roomID, eDamageType::AT_BOMB_ATTACK, target->GetHP(), damageAmount, target->GetStamina(), knockback_direction);
    }

    return true;
}

bool FITH::CheckValidPlayerPosition(Vector3f& position)
{
    if (position.y <= -15) {
        return false;
    }
    return true;
}
