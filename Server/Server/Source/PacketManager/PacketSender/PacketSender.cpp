#pragma once
#include "PacketSender.h"
#include "../../Server/Server.h"
#include "../../Room/Room.h"
#include "../../Event/Event.h"


PacketSender::~PacketSender()
{
}

void PacketSender::SendPlayerAdd(int roomID)
{
    Room* room = mServer->GetRooms()[roomID];
    std::map<int, int> teamPlayerCnt;
    room->GetPlayerListLock().lock_shared();

    for (Player* player : room->GetPlayerList()) {
        if (player == nullptr) {
            continue;
        }
        int team = player->GetTeam();

        std::vector<std::pair<int, int>>& spawnPoses = room->GetMap()->GetPlayerSpawnIndexes(player->GetTeam());

        Vector3f pos = ConvertVec2iToVec3f(spawnPoses[teamPlayerCnt[team]].first, spawnPoses[teamPlayerCnt[team]++].second);

        player->SetPosition(pos);
    }
    std::vector<uint8_t> send_buffer = mPacketMaker->MakePlayerAddPacket(room->GetPlayerList());
    room->GetPlayerListLock().unlock_shared();
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
    std::vector<uint8_t> send_buffer;
    if (inGameID == room->GetHostID()) {
        send_buffer = mPacketMaker->MakeGameMatchingResponsePacket(inGameID, roomID, team, room->GetGameMode(), room->GetPlayerLimit(), true);
    }
    else {
        send_buffer = mPacketMaker->MakeGameMatchingResponsePacket(inGameID, roomID, team, room->GetGameMode(), room->GetPlayerLimit());
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

    for (const auto& idx : unique_idx) {
        std::vector<uint8_t> send_buffer = mPacketMaker->MakeBlockDropPacket(spawnPoses[idx].first, spawnPoses[idx].second, blockType);
        mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
    }
}

void PacketSender::SendBombSpawnPacket(int roomID, int spawnCount)
{
    Room* room = mServer->GetRooms()[roomID];
    GameMode gameMode = room->GetGameMode();
    int explosionInterval = mServer->GetTableManager()-> GetGameModeData()[gameMode].Bomb_Delay_Time;

    std::set<Vector3f> spawnPoses = mServer->SetObjectSpawnPos(roomID, spawnCount);

    for (const auto& pos : spawnPoses) {
        int bombid = room->AddBomb(new Bomb, pos);
        if (bombid == INVALIDKEY) continue;
        std::vector<uint8_t> send_buffer = mPacketMaker->MakeBombSpawnPacket(pos, bombid);
        mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
        PushEventBombExplosion(mServer->GetTimer(), roomID, bombid, room->GetRoomCode(), explosionInterval);
    }
}


void PacketSender::SendBombExplosionPacket(int roomID, int bombID)
{
    mServer->GetRooms()[roomID]->GetBombListLock().lock_shared();
    Object* object = mServer->GetRooms()[roomID]->GetBombList()[bombID];
    if (object == nullptr) {
        mServer->GetRooms()[roomID]->GetBombListLock().unlock_shared();
        return;
    }
    Vector3f pos = object->GetPosition();
    mServer->GetRooms()[roomID]->GetBombListLock().unlock_shared();
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeBombExplosionPacket(bombID, pos);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendLifeReducePacket(int team, int lifeCount, int roomID) {
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeLifeReducePacket(team, lifeCount);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendRemainTimeSync(int roomID)
{
    TIMEPOINT startTime = mServer->GetRooms()[roomID]->GetStartTime();
    GameMode GameMode = mServer->GetRooms()[roomID]->GetGameMode();
    int playTime = mServer->GetTableManager()-> GetGameModeData()[GameMode].Play_Time;
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
    room->GetPlayerListLock().lock_shared();
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeGameResultPacket(winningTeams, room->GetPlayerRecordList(), room->GetPlayerList());
    room->GetPlayerListLock().unlock_shared();
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

void PacketSender::SendPlayerDeadPacket(int inGameID, int roomID)
{
    mServer->GetRooms()[roomID]->GetPlayerListLock().lock_shared();
    Player* player = dynamic_cast<Player*>(mServer->GetRooms()[roomID]->GetPlayerList()[inGameID]);
    mServer->GetRooms()[roomID]->GetPlayerListLock().unlock_shared();
    if (player == nullptr) {
        return;
    }

    std::vector<uint8_t> send_buffer = mPacketMaker->MakePlayerDeadPacket(inGameID, roomID, player->GetPosition(), player->GetDirection());
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendPlayerRespawn(int inGameID, int roomID)
{
    mServer->GetRooms()[roomID]->GetPlayerListLock().lock_shared();
    Player* player = dynamic_cast<Player*>(mServer->GetRooms()[roomID]->GetPlayerList()[inGameID]);
    mServer->GetRooms()[roomID]->GetPlayerListLock().unlock_shared();
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

void PacketSender::SendWeaponSpawnPacket(int roomID, int spawnCount, int weaponType)
{
    Room* room = mServer->GetRooms()[roomID];

    std::set<Vector3f> spawnPoses = mServer->SetObjectSpawnPos(roomID, spawnCount);

    for (const auto& pos : spawnPoses) {
        int weaponid = room->AddWeapon(new Weapon((eWeaponType)weaponType, nullptr), pos);
        if (weaponid == INVALIDKEY) continue;
        std::vector<uint8_t> send_buffer = mPacketMaker->MakeWeaponSpawnPacket(pos, weaponid, weaponType);
        mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
    }
}

void PacketSender::SendWeaponDropPacket(Vector3f position, int roomID, int weaponID)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakeWeaponDropPacket(position, weaponID);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendPlayerCalculatedDamage(int targetID, int roomID, int attackType, int hp, int damageAmount, Vector3f knockback_direction)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakePlayerCalculatedDamagePacket(targetID, attackType, hp, damageAmount, knockback_direction);
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
