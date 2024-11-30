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

void PacketSender::SendLoginResponse(int sessionID, int result, UserInfo userInfo, std::unordered_map<int, std::vector<sDayAttendanceInfo>>& attendanceInfoList)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeLoginResponsePacket(result, userInfo, attendanceInfoList);
    mServer->GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void PacketSender::SendSignUpResponse(int sessionID, int result)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeSignUpResponsePacket(result);
    mServer->GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void PacketSender::SendGachaResponsePacket(int sessionID, int result, int acquired_item_type, int acquired_item_amount, int spent_resource_type, int spent_resource_amount, int remaining_resource_amount)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeGachaResponsePacket(result, acquired_item_type, acquired_item_amount, spent_resource_type, spent_resource_amount, remaining_resource_amount);
    mServer->GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void PacketSender::SendCurrencyAmountResponsePacket(int sessionID, int result, std::vector<int>& currency_types, std::vector<int>& currency_amounts)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeCurrencyAmountResponsePacket(result, currency_types, currency_amounts);
    mServer->GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void PacketSender::SendUserItemsResponsePacket(int sessionID, int result, std::unordered_map<int, UserItem>& user_items)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeUserItemsResponsePacket(result, user_items);
    mServer->GetSessions()[sessionID]->DoSend(send_buffer.data(), send_buffer.size());
}

void PacketSender::SendAttendanceEventResponsePacket(int sessionID, int result, int eventCode, int rewardCode, int rewardAmount)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeAttendanceEventResponsePacket(result, eventCode, rewardCode, rewardAmount);
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

        std::vector<std::pair<int, int>>& spawnPoses = room->GetMap()->GetPlayerSpawnIndexes(player->GetTeam());
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

void PacketSender::SendGameMatchingResponse(int sessionID)
{
    Player* player = dynamic_cast<Player*>(mServer->GetSessions()[sessionID]);
    if (player == nullptr) {
        return;
    }
    int inGameID = player->GetInGameID();
    int roomID = player->GetRoomID();
    int team = player->GetTeam();
    Room* room = mServer->GetRooms()[roomID];
    GameMode GameMode = room->GetGameMode();
    int mapIndex = room->GetMap()->GetMapIndex();
    int mapTheme = room->GetMapTheme();

    int playTime = mServer->GetTableManager()->GetGameModeData()[mapIndex][GameMode].Play_Time;
    std::vector<uint8_t> send_buffer;
    if (inGameID == room->GetHostID()) {
        send_buffer = mPacketMaker->MakeGameMatchingResponsePacket(inGameID, roomID, team, room->GetGameMode(), mapIndex, mapTheme, playTime, room->GetGameModeOutData().Player_Count, true);
    }
    else {
        send_buffer = mPacketMaker->MakeGameMatchingResponsePacket(inGameID, roomID, team, room->GetGameMode(), mapIndex, mapTheme,  playTime, room->GetGameModeOutData().Player_Count);
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

    std::vector<std::pair<int, int>>& spawnPoses = mServer->GetRooms()[roomID]->GetMap()->GetBlockDropIndexes();
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

void PacketSender::SendPlayerRespawn(int inGameID, int roomID)
{
    int SessionID = mServer->GetRooms()[roomID]->GetPlayerList()[inGameID];
    if (SessionID == INVALIDKEY) return;
    Player* player = dynamic_cast<Player*>(mServer->GetSessions()[SessionID]);
    if (player == nullptr) {
        return;
    }
    int team = player->GetTeam();
    std::vector<std::pair<int, int>>& spawnPoses = mServer->GetRooms()[roomID]->GetMap()->GetPlayerSpawnIndexes(team);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> idx_distrib(0, spawnPoses.size() - 1);
    int idx = idx_distrib(gen);
    int posX = spawnPoses[idx].first;
    int posY = spawnPoses[idx].second;
    Vector3f pos = ConvertVec2iToVec3f(posX, posY);

    std::vector<uint8_t> send_buffer = mPacketMaker->MakePlayerRespawnPacket(inGameID, roomID, pos, player->GetHP());
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

void PacketSender::SendPlayerCalculatedDamage(int targetID, int roomID, int attackType, int hp, int damageAmount, int targetStaminaLoss, Vector3f knockback_direction)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakePlayerCalculatedDamagePacket(targetID, attackType, hp, damageAmount, targetStaminaLoss, knockback_direction);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendPlayerGroggyPacket(int inGameID, int roomID)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakePlayerGroggyPacket(inGameID);
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
