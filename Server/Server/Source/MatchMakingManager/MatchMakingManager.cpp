#pragma once
#include "MatchMakingManager.h"
#include "../Server/Server.h"
#include "../Event/Event.h"

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
        int matchedPlayerCount = tableManager->GetGameModeData()[gameMode].Player_Count;

        int roomid = pServer->CreateNewRoom(gameMode);
        if (roomid == INVALIDKEY) {
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
        MatchingComplete(roomid, sessionList);
        std::cout << "Start Game room - " << roomid << "| GameMode - " << gameMode << std::endl;
    }

    return true;
}

void MatchMakingManager::MatchingComplete(int roomID, std::vector<int>& sessions)
{
    Room* room = pServer->GetRooms()[roomID];

    room->SetPlayerCnt(sessions.size());

    // Player Add Into New Room
    for (int sessionID : sessions) {

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

            int sessionID = player->GetSessionID();
            bool AddPlayerOk = room->AddPlayer(player);
            if (AddPlayerOk == false) {
                std::cout << "AddPlayer fail: Already Player Max\n";
            }
            else {
                if (room->GetHostID() == INVALIDKEY) {
                    room->SetHost(player->GetInGameID());
                }

                if (player->GetIsBot() == true) {
                }
                else {
                    player->SetMatchingRequestTime(0);
                    pServer->GetPacketSender()->SendGameMatchingResponse(sessionID);
                    COUT << "Matched - " << sessionID << ENDL;
                }
            }
        }
        player->GetSessionStateLock().unlock();
    }

    pServer->GetPacketSender()->SendPlayerAdd(roomID);
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
            PushEventGameMatching(pServer->GetTimer(), matchingQueue.begin()->first,  matchingType, requestTime, intervalTime);
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
            PushEventGameMatching(pServer->GetTimer(), matchingQueue.begin()->first, matchingType, requestTime, intervalTime);
        }
        return;
    }

}

bool MatchingCompare::operator()(const std::pair<int, long long>& a, const std::pair<int, long long>& b) const {
    if (a.second == b.second) {
        return a.first < b.first;
    }

    return a.second < b.second;
}