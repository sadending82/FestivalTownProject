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
    int roomcnt = 0;

    while (1) {
        char command;
        std::cout << "Please Input Command" << std::endl;
        std::cin >> command;

        switch (command) {
            // ���� ����
        case GameStartCommand: {
            m_pServer->StartGame(roomcnt);
            std::cout << roomcnt << std::endl;
            roomcnt++;
        }
        break;
            // ������ ���� ����
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