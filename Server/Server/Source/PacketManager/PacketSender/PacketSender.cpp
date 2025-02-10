#pragma once
#include "PacketSender.h"
#include "../../Server/Server.h"
#include "../../Room/Room.h"
#include "../../Event/Event.h"
#include <random>
#include <map>


PacketSender::~PacketSender()
{
}

void PacketSender::SendVersionCheckResponsePacket(int sessionID, int result)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeVersionCheckResponsePacket(result);
    mServer->GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void PacketSender::SendLoginResponse(int sessionID, int result, UserInfo userInfo, std::unordered_map<int, std::set<sDayAttendanceInfo>>& attendanceInfoList, bool isNewEvent)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeLoginResponsePacket(result, userInfo, attendanceInfoList, isNewEvent);
    mServer->GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void PacketSender::SendSignUpResponse(int sessionID, int result)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeSignUpResponsePacket(result);
    mServer->GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void PacketSender::SendGachaResponsePacket(int sessionID, int result, GachaItem& result_item, GachaItem& acquired_item, int spent_resource_type, int spent_resource_amount, int remaining_resource_amount)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeGachaResponsePacket(result, result_item, acquired_item, spent_resource_type, spent_resource_amount, remaining_resource_amount);
    mServer->GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void PacketSender::SendCurrencyAmountResponsePacket(int sessionID, int result, std::vector<UserItem>& currency_list)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeCurrencyAmountResponsePacket(result, currency_list);
    mServer->GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void PacketSender::SendUserItemsResponsePacket(int sessionID, int result, std::unordered_map<int, UserItem>& user_items)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeUserItemsResponsePacket(result, user_items);
    mServer->GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void PacketSender::SendAttendanceEventResponsePacket(int sessionID, int eventCode, int dayCount, int result)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeAttendanceEventResponsePacket(result, dayCount, eventCode);
    mServer->GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void PacketSender::SendAttendanceRewardResponsePacket(int sessionID, int eventCode, int day_count, int result, int reward_item, int reward_item_count)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeAttendanceRewardResponsePacket(eventCode, day_count, result, reward_item, reward_item_count);
    mServer->GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void PacketSender::SendEventRewardResponsePacket(int sessionID, int eventCode, int result, int reward_item, int reward_item_count)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeEventRewardResponsePacket(eventCode, result, reward_item, reward_item_count);
    mServer->GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void PacketSender::SendUserPassStatePacket(int sessionID, PlayerPassInfo& playerPassState)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeUserPassStatePacket(playerPassState);
    mServer->GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void PacketSender::SendUserMissionStatePacket(int sessionID, UserMissionList& playerMissionState)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeUserMissionStatePacket(playerMissionState);
    mServer->GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void PacketSender::SendPlayerAdd(int roomID)
{
    Room* room = mServer->GetRooms()[roomID];
    std::map<int, int> teamPlayerCnt;
    std::vector<Player*> players;

    for (const auto& sID : mServer->GetRooms()[roomID]->GetPlayerList()) {
        int session_id = sID.load();
        if (session_id == INVALIDKEY) continue;
        Player* player = dynamic_cast<Player*>(mServer->GetSessions()[session_id]);
        int team = player->GetTeam();

        std::vector<std::pair<int, int>>& spawnPoses = room->GetMap().GetPlayerSpawnIndexes(player->GetTeam());
        Vector3f pos;
        if (spawnPoses.size() == 0) {
            pos = Vector3f();
        }
        else {
            pos = ConvertVec2iToVec3f(spawnPoses[teamPlayerCnt[team]].first, spawnPoses[teamPlayerCnt[team]].second);
            teamPlayerCnt[team]++;
        }

        player->SetPosition(pos);

        players.push_back(player);
    }
    std::vector<uint8_t> send_buffer = mPacketMaker->MakePlayerAddPacket(players);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendPlayerDelete(int roomID, int inGameID)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakePlayerDeletePacket(inGameID);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendGameMatchingResponse(int sessionID, int roomID)
{
    Player* player = dynamic_cast<Player*>(mServer->GetSessions()[sessionID]);
    if (player == nullptr) {
        return;
    }

    if (player->GetIsBot() == true) {
        return;
    }

    Room* room = mServer->GetRooms()[roomID];
    int inGameID = player->GetInGameID();
    int team = player->GetTeam();
    GameMode GameMode = room->GetGameMode();
    int mapIndex = room->GetMap().GetMapIndex();
    int mapTheme = room->GetMapTheme();
    int playTime = mServer->GetTableManager()->GetGameModeData()[mapIndex][GameMode].Play_Time;

    std::map<int, int> teamPlayerCnt;
    std::vector<Player*> players;

    for (const auto& sID : room->GetPlayerList()) {
        int session_id = sID.load();
        if (session_id == INVALIDKEY) continue;
        Player* player = dynamic_cast<Player*>(mServer->GetSessions()[session_id]);
        int team = player->GetTeam();

        std::vector<std::pair<int, int>>& spawnPoses = room->GetMap().GetPlayerSpawnIndexes(player->GetTeam());
        Vector3f pos;
        if (spawnPoses.size() == 0) {
            pos = Vector3f();
        }
        else {
            pos = ConvertVec2iToVec3f(spawnPoses[teamPlayerCnt[team]].first, spawnPoses[teamPlayerCnt[team]].second);
            teamPlayerCnt[team]++;
        }

        player->SetPosition(pos);

        players.push_back(player);
    }

    sMatchingInfo matchingInfo(roomID, team, (int)GameMode
        , mapIndex, mapTheme, playTime, players.size(), false);

    std::vector<uint8_t> send_buffer;
    if (inGameID == room->GetHostID()) {
        matchingInfo.IsHost = true;
        send_buffer = mPacketMaker->MakeGameMatchingResponsePacket(inGameID, matchingInfo, players);
    }
    else {
        send_buffer = mPacketMaker->MakeGameMatchingResponsePacket(inGameID, matchingInfo, players);
    }

    mServer->GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void PacketSender::SendGameStart(int roomID)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeGameStartPacket(roomID);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendAllPlayerReady(int roomID)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeAllPlayerReadyPacket();
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendHeartBeatPacket(int sessionID)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeHeartBeatPacket();
    mServer->GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void PacketSender::SendBlockDropPacket(int roomID, int spawnCount, int blockType)
{
    Room* room = mServer->GetRooms()[roomID];
    if (room->GetState() == eRoomState::RS_FREE) {
        return;
    }

    std::vector<std::pair<int, int>>& spawnPoses = mServer->GetRooms()[roomID]->GetMap().GetBlockDropIndexes();
    GameMode gameMode = mServer->GetRooms()[roomID]->GetGameMode();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> idx_distrib(0, spawnPoses.size() - 1);

    std::set<int> unique_idx;

    while (unique_idx.size() < spawnCount) {
        int idx = idx_distrib(gen);
        unique_idx.emplace(idx);
    }

    std::vector<std::pair<int, int>> spawn_position;
    std::vector<int> blockTypes;

    for (const auto& idx : unique_idx) {
        spawn_position.push_back(std::make_pair(spawnPoses[idx].first, spawnPoses[idx].second));
        blockTypes.push_back(blockType);
    }
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeBlockDropPacket(spawn_position, blockTypes);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendBombSpawnPacket(std::vector<Vector3f>& positions, std::vector<int>& bombIDs, int explosionInterval, int roomID)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeBombSpawnPacket(positions, bombIDs, explosionInterval);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}


void PacketSender::SendBombExplosionPacket(int roomID, int bombID)
{
    Bomb* bomb = mServer->GetRooms()[roomID]->GetBomb(bombID);
    if (bomb == nullptr) {
        return;
    }
    Vector3f pos = bomb->GetPosition();
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeBombExplosionPacket(bombID, pos);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendLifeReducePacket(int team, int lifeCount, int roomID) {
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeLifeReducePacket(team, lifeCount);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendRemainTimeSync(int roomID)
{
    Room* room = mServer->GetRooms()[roomID];
    TIMEPOINT startTime = room->GetStartTime();
    GameMode GameMode = room->GetGameMode();
    int mapIndex = room->GetmapIndex();
    int playTime = mServer->GetTableManager()->GetGameModeData()[mapIndex][GameMode].Play_Time;
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeRemainTimeSyncPacket(roomID, startTime, playTime);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendGameStartPacket(int roomID)
{
    long startTime = 0;
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeGameStartPacket(roomID, startTime);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendGameEndPacket(int roomID, uint8_t winningTeams_flag)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeGameEndPacket(winningTeams_flag);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendGameResultPacket(int roomID, std::set<int>& winningTeams)
{
    Room* room = mServer->GetRooms()[roomID];

    std::unordered_map<int, Player*> players;

    for (const auto& sID : mServer->GetRooms()[roomID]->GetPlayerList()) {
        int session_id = sID.load();
        if (session_id == INVALIDKEY) continue;
        Player* player = dynamic_cast<Player*>(mServer->GetSessions()[session_id]);
        players[player->GetInGameID()] = player;
    }

    std::vector<uint8_t> send_buffer = mPacketMaker->MakeGameResultPacket(winningTeams, room->GetPlayerRecordList(), players);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendGameHostChange(int sessionID)
{
    if (sessionID == INVALIDKEY) {
        return;
    }
    Player* player = dynamic_cast<Player*>(mServer->GetSessions()[sessionID]);
    if (player == nullptr) {
        return;
    }
    int inGameID = player->GetInGameID();
    int roomID = player->GetRoomID();
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeGameHostChangePacket(inGameID, roomID);

    player->DoSend(send_buffer.data(), send_buffer.size());
}

void PacketSender::SendPlayerDeadPacket(int inGameID, int roomID, int spawn_delay)
{
    int SessionID = mServer->GetRooms()[roomID]->GetPlayerList()[inGameID];
    if (SessionID == INVALIDKEY) return;
    Player* player = dynamic_cast<Player*>(mServer->GetSessions()[SessionID]);
    if (player == nullptr) {
        return;
    }

    std::vector<uint8_t> send_buffer = mPacketMaker->MakePlayerDeadPacket(inGameID, roomID, player->GetPosition(), player->GetDirection(), spawn_delay);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendPlayerRespawn(int inGameID, int roomID, Vector3f spawnPos)
{
    int SessionID = mServer->GetRooms()[roomID]->GetPlayerList()[inGameID];
    if (SessionID == INVALIDKEY) return;
    Player* player = dynamic_cast<Player*>(mServer->GetSessions()[SessionID]);
    if (player == nullptr) {
        return;
    }
    int team = player->GetTeam();

    std::vector<uint8_t> send_buffer = mPacketMaker->MakePlayerRespawnPacket(inGameID, roomID, spawnPos, player->GetHP());
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendWeaponSpawnPacket(std::vector<Vector3f>& positions, std::vector<int>& weaponIDs, std::vector<int>& weaponTypes, int roomID)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeWeaponSpawnPacket(positions, weaponIDs, weaponTypes);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendBombDropPacket(Vector3f position, int roomID, int bombID)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeBombDropPacket(position, roomID, bombID);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}


void PacketSender::SendWeaponDropPacket(Vector3f position, int roomID, int weaponID)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeWeaponDropPacket(position, weaponID);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendPlayerCalculatedDamage(int targetID, int roomID, int attackType, int hp, int damageAmount, int stamina, Vector3f knockback_direction)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakePlayerCalculatedDamagePacket(targetID, attackType, hp, damageAmount, stamina, knockback_direction);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendPlayerGroggyPacket(int inGameID, int roomID, int playerHP)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakePlayerGroggyPacket(inGameID, playerHP);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendPlayerGroggyRecoveryPacket(int inGameID, int roomID, int recoveredStamina)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakePlayerGroggyRecoveryPacket(inGameID, recoveredStamina);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendPlayerThrowOtherPlayerPacket(int roomID, int inGameID, Vector3f myPosition, Vector3f myDirection, int targetID, Vector3f targetPosition, Vector3f targetDirection)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakePlayerThrowOtherPlayerPacket(inGameID, myPosition, myDirection, targetID, targetPosition, targetDirection);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}
