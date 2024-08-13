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
            m_pServer->StartGame(roomcnt);
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

#endif RunTest