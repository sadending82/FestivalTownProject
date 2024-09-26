#pragma once
#include "TestThread.h"
#include"../../Server/Server.h"

#include <iostream>
#include <random>

#ifdef RunTest

TestThread::~TestThread()
{
    delete m_pPacketManager;
}

void TestThread::RunWorker()
{
    m_pPacketManager = new PacketManager();
    m_pPacketManager->Init(m_pServer);

    AllocConsole();
    FILE* fpOut;
    freopen_s(&fpOut, "CONOUT$", "w", stdout);
    freopen_s(&fpOut, "CONIN$", "r", stdin);

    while (isRun) {
        char command;
        std::cout << "Please Input Command (s = GameStart)" << std::endl;
        std::cin >> command;

        switch (command) {
            // 게임 시작
        case GameStartCommand: {
            int playerCnt = GetReadyPlayerCnt();
            if (playerCnt == 0) {
                continue;
            }
            std::priority_queue<std::pair<int, Player*>
                , std::vector<std::pair<int, Player*>>
                , std::greater<std::pair<int, Player*>>> readyPlayers;

            for (Session* s :m_pServer->GetSessions()) {
                s->GetStateLock().lock();
                if (s->GetState() == eSessionState::ST_MATCHWAITING) {
                    readyPlayers.push({ s->GetMatchingRequestTime(), dynamic_cast<Player*>(s) });
                }
                s->GetStateLock().unlock();
            }

            int waitingPlayerCount = readyPlayers.size();
            std::unordered_map<GameMode, GameModeInfo>& gameModeInfos = m_pServer->GetTableManager()->GetGameModeData();

            while (!readyPlayers.empty()) {
                GameMode gameMode = GameMode::FITH_Team_Battle_6;

                for (auto iter = gameModeInfos.begin(); iter != gameModeInfos.end(); iter++) {
                    if (waitingPlayerCount == iter->second.Player_Count) {
                        gameMode = iter->first;
                        break;
                    }
                }
                int playerCount = gameModeInfos[gameMode].Player_Count;

                int roomid = m_pServer->CreateNewRoom(playerCount, gameMode);
                if (roomid == INVALIDKEY) {
                    std::cout << "Fali Create New Room\n";
                    break;
                }

                std::vector<Player*> playerList;
                int matchingPlayerCount = MAXPLAYER;
                for (int i = 0; i < matchingPlayerCount; ++i) {
                    if (readyPlayers.empty()) {
                        break;
                    }
                    playerList.push_back(readyPlayers.top().second);
                    readyPlayers.pop();
                }
                waitingPlayerCount = readyPlayers.size();
                m_pServer->MatchingComplete(roomid, playerList);
                std::cout << "Start Game room - " << roomid << "| GameMode - " << gameMode << std::endl;
            }

            PushEventGameMatching(m_pServer->GetTimer());
        }
        break;
            // 라이프 감소 전송
        case SendLifeReduceCommand: {
            int roomID;
            std::cout << "Input roomID" << std::endl;
            std::cin >> roomID;
            m_pServer->GetPacketSender()->SendLifeReducePacket(0, 0, roomID);
        }
        break;
        case DeleteRoom: {
            int roomID;
            std::cout << "Input roomID" << std::endl;
            std::cin >> roomID;
            m_pServer->GetRooms()[roomID]->Reset();
            std::cout << "Delete room - " << roomID << std::endl;
        }
        break;

        case Gacha: {
            int randomBox;
            std::cout << "Input Random Box Index\n";
            std::cin >> randomBox;

            TableManager* tableManager = m_pServer->GetTableManager();

            if (tableManager->GetRandomBoxList().find(randomBox) == tableManager->GetRandomBoxList().end()) {
                break;
            }
            int GachaGroup = tableManager->GetRandomBoxList()[randomBox].Gacha_Group;
            std::unordered_map<INDEX, GachaItem>& items = tableManager->GetGachaItemList()[GachaGroup];

            int totalWeight = 0;

            for (auto& item : items) {
                totalWeight += item.second.Gacha_Weight;
            }

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, totalWeight);
            int randomWeight = dis(gen);
            int cumulativeWeight = 0;
            int rewardItem = 0;
            int rewardValue = 0;

            for (auto& item : items) {
                cumulativeWeight += item.second.Gacha_Weight;
                if (randomWeight <= cumulativeWeight) {
                    rewardItem = item.second.Reward_Item_Index;
                    rewardValue = item.second.Reward_Item_Value;
                    break;
                }

            }

            std::cout << "Gacha Results!" << " -> Item: " << rewardItem  << " | Value: " << rewardValue <<std::endl;
        }
        break;
        default: {
            std::cout << "Wrong Command" << std::endl;
        }
        break;
        }
    }
}

int TestThread::GetReadyPlayerCnt()
{
    int cnt = 0;
    for (Session* s : m_pServer->GetSessions()) {
        s->GetStateLock().lock();
        if (s->GetState() == eSessionState::ST_MATCHWAITING) {
            cnt++;
        }
        s->GetStateLock().unlock();

        if (cnt >= 6) return cnt;
    }
    return cnt;
}

#endif RunTest