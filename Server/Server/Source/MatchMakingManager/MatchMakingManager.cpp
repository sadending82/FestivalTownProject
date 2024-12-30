#pragma once
#include "MatchMakingManager.h"
#include "../Server/Server.h"
#include "../Event/Event.h"
#include <random>
#include <thread>

using namespace std::chrono;

MatchMakingManager::MatchMakingManager(Server* server)
{
    pServer = server;
    pMatchingDataTables = &pServer->GetTableManager()->GetMatchingDatas();
    mMatchingSequence[eMatchingType::FITH_SOLO] = eMatchingSequence::MS_None;
    mMatchingSequence[eMatchingType::FITH_TEAM] = eMatchingSequence::MS_None;
}

MatchMakingManager::~MatchMakingManager()
{

}

void MatchMakingManager::RunMatchingThreadWorker() {

    while (1) {
        mMatchingLock.lock();
        for (auto pair : mMatchingSequence) {

            eMatchingType type = pair.first;

            eMatchingSequence matchingSequence = pair.second;

            if (matchingSequence == eMatchingSequence::MS_None) {

            }
            else {
                CheckMatchMaking(type);
            }
            UpdateMatchingSequence(type);

        }
        mMatchingLock.unlock();
        std::this_thread::sleep_for(100ms);
    }
}

bool MatchMakingManager::CheckMatchMaking(eMatchingType matchingType)
{
    MATCHING_QUEUE& matchingQueue = mMatchingQueue[matchingType];
    int waitingPlayerCount = matchingQueue.size();

    TableManager* tableManager = pServer->GetTableManager();

    int matchingSequence = mMatchingSequence[matchingType];

    if (matchingSequence == eMatchingSequence::MS_None) {
        return false;
    }

    Matching_Table& matchingTable = pMatchingDataTables->find(matchingType)->second[matchingSequence];

    if (waitingPlayerCount < matchingTable.Recruit_Player) {
        return false;
    }

    std::vector<int> matchingModes = { matchingTable.Mode1_Index, matchingTable.Mode2_Index };

    GAMEMANAGER_MAP& gameManagers = pServer->GetGameManagers();

    for (int mode : matchingModes) {

        if (mode == 0) {
            continue;
        }

        GameMode gameMode = (GameMode)(mode);
        int matchedPlayerCount = tableManager->GetGameModeOutData()[gameMode].Player_Count;

        MapProperties mapProperties = SelectRandomMap(gameMode);

        int roomID = pServer->CreateNewRoom(gameMode, mapProperties.Map_Index, mapProperties.Map_Theme);
        if (roomID == INVALIDKEY) {
            std::cout << "Fali Create New Room\n";
            break;
        }

        std::vector<int> sessionList;
        for (int i = 0; i < matchedPlayerCount; ++i) {
            if (matchingQueue.empty()) {
                break;
            }
            int topPlayer = matchingQueue.begin()->first;

            sessionList.push_back(topPlayer);
            matchingQueue.erase(matchingQueue.begin());
        }

        MatchingComplete(roomID, sessionList);
        std::cout << "Start Game room - " << roomID << "| GameMode - " << gameMode << std::endl;
    }

    return true;
}

void MatchMakingManager::MatchingComplete(int roomID, std::vector<int>& sessionIDs)
{
    Room* room = pServer->GetRooms()[roomID];

    PacketSender* packetSender = pServer->GetPacketSender();

    room->SetPlayerCnt(9999);

    int addCompletePlayerCnt = 0;

    // Player Add Into New Room
    for (int sessionID : sessionIDs) {
        Player* player = dynamic_cast<Player*>(pServer->GetSessions()[sessionID]);
        player->GetSessionStateLock().lock();
        if (player->GetSessionState() == eSessionState::ST_MATCHWAITING) {
            // 인원수가 적은 팀에 배치
            int teamNum = 987654321;
            int minTeammateCnt = 987654321;
            for (auto& pair : room->GetTeams()) {
                int key = pair.first;
                Team& currTeam = pair.second;

                int teammateCnt = currTeam.GetMembers().size();

                if (teammateCnt < minTeammateCnt || (teammateCnt == minTeammateCnt && key < teamNum)) {
                    teamNum = key;
                    minTeammateCnt = teammateCnt;
                }
            }

            player->SetTeam(teamNum);
            player->ChangeCharacterType(pServer, eCharacterType::CT_TEST);
            player->SetHP(player->GetCharacterStat().hp);
            player->SetStamina(player->GetCharacterStat().stamina);

            bool AddPlayerOk = room->AddPlayer(player);
            if (AddPlayerOk == false) {
                std::cout << "AddPlayer fail: Already Player Max\n";
            }
            else {
                if (room->GetHostID() == INVALIDKEY) {
                    room->SetHost(player->GetInGameID());
                }

                if (player->GetIsBot() == true) {
                    room->SetIsPlayerReady(player->GetInGameID(), true);
                }
                else {
                    player->SetMatchingRequestTime(0);
                    //COUT << "Matched - " << sessionID << ENDL;
                }
                addCompletePlayerCnt++;
            }
        }
        player->GetSessionStateLock().unlock();
    }

    room->SetPlayerCnt(addCompletePlayerCnt);

    for (const auto& sID : room->GetPlayerList()) {
        int id = sID.load();
        if (id == INVALIDKEY) continue;
        packetSender->SendGameMatchingResponse(sID, roomID);
    }

    //packetSender->SendPlayerAdd(roomID);
}

void MatchMakingManager::UpdateMatchingSequence(eMatchingType matchingType)
{
    MATCHING_QUEUE& matchingQueue = mMatchingQueue[matchingType];

    if (matchingQueue.empty()) {
        mMatchingSequence[matchingType] = eMatchingSequence::MS_None;
        return;
    }

    int sessionID = matchingQueue.begin()->first;

    Player* player = dynamic_cast<Player*>(pServer->GetSessions()[sessionID]);

    std::vector<Matching_Table>& matchingTable = pMatchingDataTables->find(matchingType)->second;

    long long currTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    long long requestTime = player->GetMatchingRequestTime();
    long long waitingTime = currTime - player->GetMatchingRequestTime();


    if (matchingTable[eMatchingSequence::MS_Last].Matching_Min_Time <= waitingTime ) {

        if (mMatchingSequence[matchingType] == eMatchingSequence::MS_Last) {
            return;
        }

        mMatchingSequence[matchingType] = eMatchingSequence::MS_Last;

        // 현재 대기 중인 플레이어로 매칭 되는지 확인 후 다른 플레이어 매칭 요청 전까지 무한 대기
        CheckMatchMaking(matchingType);
        UpdateMatchingSequence(matchingType);

        return;
    } 

    if (matchingTable[eMatchingSequence::MS_2].Matching_Min_Time <= waitingTime) {

        if (mMatchingSequence[matchingType] == eMatchingSequence::MS_2) {
            return;
        }

        mMatchingSequence[matchingType] = eMatchingSequence::MS_2;

        int intervalTime = matchingTable[eMatchingSequence::MS_2].Matching_Max_Time - waitingTime;

        // 현재 대기 중인 플레이어로 매칭 되는지 확인
        bool matchingResult = CheckMatchMaking(matchingType);
        if (matchingResult == true) {
            UpdateMatchingSequence(matchingType);
        }
        else if (matchingResult == false) {
            // 매칭 실패하면 다음 시퀀스 대기
            //PushEventGameMatching(pServer->GetTimer(), matchingQueue.begin()->first,  matchingType, requestTime, intervalTime);
        }

        return;
    }

    if (matchingTable[eMatchingSequence::MS_1].Matching_Min_Time <= waitingTime) {

        if (mMatchingSequence[matchingType] == eMatchingSequence::MS_1) {
            return;
        }

        mMatchingSequence[matchingType] = eMatchingSequence::MS_1;

        int intervalTime = matchingTable[eMatchingSequence::MS_1].Matching_Max_Time - waitingTime;

        // 현재 대기 중인 플레이어로 매칭 되는지 확인
        bool matchingResult = CheckMatchMaking(matchingType);
        if (matchingResult == true) {
            UpdateMatchingSequence(matchingType);
        }
        else if (matchingResult == false) {
            // 매칭 실패하면 다음 시퀀스 대기
            //PushEventGameMatching(pServer->GetTimer(), matchingQueue.begin()->first, matchingType, requestTime, intervalTime);
        }
        return;
    }

}

MapProperties MatchMakingManager::SelectRandomMap(GameMode gameMode)
{
    MapProperties result;

    TableManager* tableManager = pServer->GetTableManager();

    std::vector<int>& mapList = tableManager->getMapListByMode()[gameMode];
    std::unordered_map<int, std::vector<int>> themeList = tableManager->GetMapThemeList();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<>map_distrib(0, mapList.size() - 1);

    result.Map_Index = mapList[map_distrib(gen)];

    std::uniform_int_distribution<>theme_distrib(0, themeList[result.Map_Index].size() - 1);

    result.Map_Theme = themeList[result.Map_Index][theme_distrib(gen)];

    return result;
}

bool MatchingCompare::operator()(const std::pair<int, long long>& a, const std::pair<int, long long>& b) const {
    if (a.second == b.second) {
        return a.first < b.first;
    }

    return a.second < b.second;
}