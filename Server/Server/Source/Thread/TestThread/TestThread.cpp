#include "TestThread.h"
#include"../../Server/Server.h"

#ifdef RunTest

void TestThread::RunWorker()
{
    m_pPacketManager = new PacketManager();
    m_pPacketManager->Init(m_pServer);

    AllocConsole();
    FILE* fpOut;
    freopen_s(&fpOut, "CONOUT$", "w", stdout);
    freopen_s(&fpOut, "CONIN$", "r", stdin);

    while (1) {
        char command;
        std::cout << "Please Input Command" << std::endl;
        std::cin >> command;
        int roomcnt = 0;

        switch (command) {
            // 게임 시작
        case GameStartCommand: {
            GameStart(roomcnt);
            roomcnt++;
        }
        break;
            // 라이프 감소 전송
        case SendLifeReduceCommand: {
            int roomID;
            std::cout << "Input roomID" << std::endl;
            std::cin >> roomID;
            m_pServer->SendLifeReducePacket(0, 0, roomID);
        }
        break;
        default: {
            std::cout << "Wrong Command" << std::endl;
        }
        break;
        }
    }
}

void TestThread::GameStart(int roomID)
{
    // Activate New Room
    m_pServer->GetRooms()[roomID]->Init(roomID, m_pServer->GetTableManager()->getFITH_Data()[GameCode::FITH_Team_battle_Three].Team_Life_Count);
    m_pServer->GetRooms()[roomID]->SetGameMode(GameCode::FITH_Team_battle_Three);
    m_pServer->GetRooms()[roomID]->InitMap(m_pServer->GetTableManager()->getMapData()[TEST]);
    m_pServer->GetRooms()[roomID]->SetPlayerLimit(6); // 임시

    // Player Add Into New Room
    for (Session* s : m_pServer->GetSessions()) {
        if (m_pServer->GetRooms()[roomID]->GetPlayerCnt() == m_pServer->GetRooms()[roomID]->GetPlayerLimit()) {
            break;
        }
        if (s->GetState() == eSessionState::ST_ACCEPTED) {
            Player* p = dynamic_cast<Player*>(s);
            int sessionID = p->GetSessionID();
            bool addPlayerOk = m_pServer->GetRooms()[roomID]->addPlayer(p);
            if (addPlayerOk == false) {
                std::cout << "AddPlayer fail: Already Player Max\n";
            }
            else {
                m_pServer->GetRooms()[TESTROOM]->AddPlayerCnt();
                m_pServer->SendPlayerGameInfo(sessionID);
            }
        }
    }

    // Send Each Player's Info
    for (Player* p : m_pServer->GetRooms()[roomID]->GetPlayerList()) {
        if (p == nullptr) continue;
        for (Player* other : m_pServer->GetRooms()[roomID]->GetPlayerList()) {
            if (other == nullptr) continue;
            m_pServer->SendPlayerAdd(p->GetSessionID(), other->GetSessionID());
        }
    }

    // Push Event
    GameCode gameCode = m_pServer->GetRooms()[roomID]->GetGameMode();
    int eventTime = m_pServer->GetTableManager()->getFITH_Data()[gameCode].Block_Spawn_Time;
    PushEventObjectDrop(m_pTimer, roomID, eventTime);
    PushEventBombSpawn(m_pTimer, roomID, m_pServer->GetTableManager()->getFITH_Data()[gameCode].Bomb_Spawn_Time);
}

#endif RunTest