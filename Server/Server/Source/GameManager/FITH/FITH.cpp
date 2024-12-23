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
        if (rewards.Win_Reward1_Index != 0) {
            reward1.index = rewards.Win_Reward1_Index;
            reward1.value += rewards.Win_Reward1_Value;
            if (isMvp) {
                reward1.value += BonusRewards.MVP_Reward1_Value;
            }
            else {
                reward1.value += BonusRewards.Win_Reward1_Value;
            }
        }

        if (rewards.Win_Reward2_Index != 0) {
            reward2.index = rewards.Win_Reward2_Index;
            reward2.value += rewards.Win_Reward2_Value;
            if (isMvp) {
                reward2.value += BonusRewards.MVP_Reward2_Value;
            }
            else {
                reward2.value += BonusRewards.Win_Reward2_Value;
            }
        }

        if (rewards.Win_Reward3_Index != 0) {
            reward3.index = rewards.Win_Reward3_Index;
            reward3.value += rewards.Win_Reward3_Value;
            if (isMvp) {
                reward3.value += BonusRewards.MVP_Reward3_Value;
            }
            else {
                reward3.value += BonusRewards.Win_Reward3_Value;
            }
        }
    }
             break;

    case BattleResult::BR_Lose: {
        if (rewards.Lose_Reward1_Index != 0) {
            reward1.index = rewards.Lose_Reward1_Index;
            reward1.value += rewards.Lose_Reward1_Value + BonusRewards.Lose_Reward1_Value;
        }
        if (rewards.Lose_Reward2_Index != 0) {
            reward2.index = rewards.Lose_Reward2_Index;
            reward2.value += rewards.Lose_Reward2_Value + BonusRewards.Lose_Reward2_Value;
        }
        if (rewards.Lose_Reward3_Index != 0) {
            reward3.index = rewards.Lose_Reward3_Index;
            reward3.value += rewards.Lose_Reward3_Value + BonusRewards.Lose_Reward3_Value;
        }
    }
              break;
    case BattleResult::BR_Draw: {
        if (rewards.Draw_Reward1_Index != 0) {
            reward1.index = rewards.Draw_Reward1_Index;
            reward1.value += rewards.Draw_Reward1_Value + BonusRewards.Draw_Reward1_Value;
        }
        if (rewards.Draw_Reward2_Index != 0) {
            reward2.index = rewards.Draw_Reward2_Index;
            reward2.value += rewards.Draw_Reward2_Value + BonusRewards.Draw_Reward2_Value;
        }
        if (rewards.Draw_Reward3_Index != 0) {
            reward3.index = rewards.Draw_Reward3_Index;
            reward3.value += rewards.Draw_Reward3_Value + BonusRewards.Draw_Reward3_Value;
        }
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
        pair.second.rewards = CalculateGameReward(point, false, result);

        if (point > mvp_point) {
            mvp_id = id;
            mvp_point = point;
        }

    }

    if (winFlag == BattleResult::BR_Win) {
        if (mvp_id != INVALIDKEY) {
            records[mvp_id].rewards = CalculateGameReward(records[mvp_id].gameRecord.Point, true, BattleResult::BR_Win);
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

        for (int i = 0; i < record.rewards.size(); ++i) {
            if (record.rewards[i].index == 0) {
                continue;
            }
            pDB->UpsertUserItemCount(uid, record.rewards[i].index, record.rewards[i].value);
        }
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
            COUT << "ERROR!: " << "GetBomb FAIL. Bomb ID: " << id << ENDL;
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

    if (targetID == attackerID) {
        return false;
    }
    // 팀킬 방지
    if (target->GetTeam() == attacker->GetTeam()) {
        return false;
    }

    target->GetPlayerStateLock().lock();
    // 살아있는지 확인
    if (target->GetPlayerState() != ePlayerState::PS_ALIVE) {
        target->GetPlayerStateLock().unlock();
        return false;
    }
    target->GetPlayerStateLock().unlock();


    // 타겟의 기력이 없으면 그로기 상태로
    if (target->GetStamina() == 0) {
        if (target->ChangeToGroggyState(pServer)) {
            targetGameRecord.gameRecord.Groggy_Count.fetch_add(1);
            PushEventGroggyRecovery(pServer->GetTimer(), targetID, roomID, room->GetRoomCode(), room->GetGameModeData().Ch_Groggy);
        }
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
    target->ReduceStamina(TargetStaminaLoss);


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
        pPacketSender->SendPlayerCalculatedDamage(targetID, roomID, eDamageType::AT_ATTACK, target->GetHP(), damageAmount, TargetStaminaLoss, knockback_direction);

        // 타격 후 스테미너가 0미만이면 그로기로
        if (target->GetStamina() < 0) {
            if (target->ChangeToGroggyState(pServer)) {
                targetGameRecord.gameRecord.Groggy_Count.fetch_add(1);
                PushEventGroggyRecovery(pServer->GetTimer(), targetID, roomID, room->GetRoomCode(), room->GetGameModeData().Ch_Groggy);
            }
        }
    }
    attacker->GetWeaponLock().unlock_shared();

    return true;
}

bool FITH::PlayerDamagedFromBomb(int roomID, Room* room, int targetID, Player* target, Vector3f knockback_direction)
{
    sPlayerGameRecord& targetGameRecord = room->GetPlayerRecordList().at(targetID);

    target->GetPlayerStateLock().lock();
    // 살아있는지 확인
    if (target->GetPlayerState() != ePlayerState::PS_ALIVE) {
        target->GetPlayerStateLock().unlock();
        return false;
    }
    target->GetPlayerStateLock().unlock();


    int damageAmount = room->GetGameModeData().Bomb_Damage;

    // 타겟의 기력이 없으면 그로기 상태로
    if (target->GetStamina() == 0) {
        if (target->ChangeToGroggyState(pServer)) {
            PushEventGroggyRecovery(pServer->GetTimer(), targetID, roomID, room->GetRoomCode(), room->GetGameModeData().Ch_Groggy);
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
        pPacketSender->SendPlayerCalculatedDamage(targetID, roomID, eDamageType::AT_BOMB_ATTACK, target->GetHP(), damageAmount, 0, knockback_direction);
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
