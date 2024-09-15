#include "TestThread.h"
#include"../../Server/Server.h"

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
            std::queue<Player*> readyPlayers;
            for (Session* s : m_pServer->GetSessions()) {
                s->GetStateLock().lock();
                if (s->GetState() == eSessionState::ST_MATCHWAITING) {
                    readyPlayers.push(dynamic_cast<Player*>(s));
                }
                s->GetStateLock().unlock();
            }

            int playerCount = readyPlayers.size();

            while (!readyPlayers.empty()) {
                int roomid = m_pServer->CreateNewRoom((playerCount > MAXPLAYER) ? MAXPLAYER : playerCount, GameMode::FITH_Team_battle_Three);
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
                    playerList.push_back(readyPlayers.front());
                    readyPlayers.pop();
                    playerCount--;
                }

                m_pServer->MatchingComplete(roomid, playerCount, playerList);
                std::cout << "Start Game room - " << roomid << std::endl;
            }
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