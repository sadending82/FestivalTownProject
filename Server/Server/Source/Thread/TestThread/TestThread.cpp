#pragma once
#include "TestThread.h"
#include"../../Server/Server.h"

#include <iostream>
#include <random>

#ifdef RunTest

TestThread::~TestThread()
{

}

void TestThread::RunWorker()
{
    AllocConsole();
    FILE* fpOut;
    freopen_s(&fpOut, "CONOUT$", "w", stdout);
    freopen_s(&fpOut, "CONIN$", "r", stdin);

    while (isRun) {
        char command;
        std::cout << "Please Input Command" << std::endl;
        std::cin >> command;

        switch (command) {
            // 게임 시작
        case GameStartCommand: {

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
            m_pServer->GetRooms().at(roomID)->Reset();
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
        case TableReload: {
            m_pServer->TableReLoad();
            std::cout << "Table Load Complete\n";
        }
                        break;
        default: {
            std::cout << "Wrong Command" << std::endl;
        }
        break;
        }
    }
}
#endif RunTest