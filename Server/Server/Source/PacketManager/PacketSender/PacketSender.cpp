#pragma once
#include "PacketSender.h"
#include "../../Server/Server.h"
#include "../../Room/Room.h"
#include "../../Event/Event.h"


PacketSender::~PacketSender()
{
}

void PacketSender::SendPlayerAdd(int sessionID, int destination)
{
    Player* player = dynamic_cast<Player*>(mServer->GetSessions()[sessionID]);
    if (player == nullptr) {
        return;
    }
    int inGameID = player->GetInGameID();
    int roomID = player->GetRoomID();

    std::vector<std::pair<int, int>>& spawnPoses = mServer->GetRooms()[roomID]->GetMap()->GetPlayerSpawnIndexes(player->GetTeam());

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> idx_distrib(0, spawnPoses.size() - 1);
    int idx = idx_distrib(gen);
    int posX = spawnPoses[idx].first;
    int posY = spawnPoses[idx].second;

    std::vector<uint8_t> send_buffer = mPacketMaker->MakePlayerAdd(inGameID, Vector3f(posX, posY, 0));

    mServer->GetSessions()[destination]->DoSend(send_buffer.data(), send_buffer.size());
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

void PacketSender::SendBlockDropPacket(int roomID, int spawnCount)
{
    std::vector<std::pair<int, int>>& spawnPoses = mServer->GetRooms()[roomID]->GetMap()->GetBlockDropIndexes();
    GameCode gameMode = mServer->GetRooms()[roomID]->GetGameMode();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> idx_distrib(0, spawnPoses.size() - 1);
    std::uniform_int_distribution<> type_distrib(0, 1);

    std::set<int> unique_idx;

    while (unique_idx.size() < spawnCount) {
        int idx = idx_distrib(gen);
        unique_idx.emplace(idx);
    }

    for (const auto& idx : unique_idx) {
        std::vector<uint8_t> send_buffer = mPacketMaker->MakeBlockDropPacket(spawnPoses[idx].first, spawnPoses[idx].second, type_distrib(gen));
        mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
    }
}

void PacketSender::SendBombSpawnPacket(int roomID, int spawnCount)
{
    Room* room = mServer->GetRooms()[roomID];
    GameCode gameMode = room->GetGameMode();
    int explosionInterval = mServer->GetTableManager()->getFITH_Data()[gameMode]->Bomb_Delay_Time;

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
    mServer->GetRooms()[roomID]->GetObjectListLock().lock_shared();
    Object* object = mServer->GetRooms()[roomID]->GetObjects()[bombID];
    if (object == nullptr) {
        mServer->GetRooms()[roomID]->GetObjectListLock().unlock_shared();
        return;
    }
    Vector3f pos = object->GetPosition();
    mServer->GetRooms()[roomID]->GetObjectListLock().unlock_shared();
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
    GameCode gameCode = mServer->GetRooms()[roomID]->GetGameMode();
    int playTime = mServer->GetTableManager()->getFITH_Data()[gameCode]->Play_Time;
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

    std::vector<uint8_t> send_buffer = mPacketMaker->MakePlayerRespawnPacket(inGameID, roomID, posX, posY, player->GetHP());
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}

void PacketSender::SendWeaponSpawnPacket(int roomID, int spawnCount)
{
    Room* room = mServer->GetRooms()[roomID];

    std::set<Vector3f> spawnPoses = mServer->SetObjectSpawnPos(roomID, spawnCount);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> type_distrib((int)eWeaponType::WT_FRYING_PAN, (int)eWeaponType::WT_BAT);

    for (const auto& pos : spawnPoses) {
        int type = type_distrib(gen);
        int weaponid = room->AddWeapon(new Weapon((eWeaponType)type, nullptr), pos);
        if (weaponid == INVALIDKEY) continue;
        std::vector<uint8_t> send_buffer = mPacketMaker->MakeWeaponSpawnPacket(pos, weaponid, type_distrib(gen));
        mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
    }
}

void PacketSender::SendPlayerCalculatedDamage(int targetID, int roomID, int attackType, int hp, int damageAmount, Vector3f knockback_direction)
{
    std::vector<uint8_t> send_buffer = mPacketMaker->MakePlayerCalculatedDamagePacket(targetID, attackType, hp, damageAmount, knockback_direction);
    mServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
}