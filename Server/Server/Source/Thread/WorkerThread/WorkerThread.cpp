#include "WorkerThread.h"
#include"../../Server/Server.h"

WorkerThread::~WorkerThread()
{
    delete pPacketManager;
}

void WorkerThread::RunWorker()
{
    pPacketManager = new PacketManager();
    pPacketManager->Init(pServer);

    while (isRun) {
        DWORD Transferred;
        ULONG key;
        WSAOVERLAPPED* over;

        BOOL retval = GetQueuedCompletionStatus(pServer->GetHcp(), &Transferred,
            (PULONG_PTR)&key, &over, INFINITE);

        ExOver* exOver = reinterpret_cast<ExOver*>(over);

        if (false == retval && Transferred == 0) {
            pServer->Disconnect(key);
        }

        switch (exOver->mOpType) {
        case eOpType::OP_ACCEPT: {
            int newKey = pServer->SetSessionID();
            if (newKey != INVALIDKEY) {
                Session* newSession = pServer->GetSessions()[newKey];
                SOCKET cSocket = reinterpret_cast<SOCKET>(exOver->mWsaBuf.buf);
                /*newSession->SetSocket(cSocket);
                newSession->GetExOver().SetmOpType(eOpType::OP_RECV);
                newSession->SetPrevData(0);
                newSession->SetSessionID(newKey);
                newSession->SetPlayedSoloGameBefore(false);*/
                newSession->SessionInit(cSocket, newKey);

                CreateIoCompletionPort((HANDLE)newSession->GetSocket(), pServer->GetHcp(), newKey, 0);
                DEBUGMSGONEPARAM("Lobby Accept: %d\n", newKey);

                // 접속하자 마자 HeartBeat 이벤트 추가
                pServer->StartHeartBeat(newKey);

                newSession->DoRecv();
            }
            else {
                DEBUGMSGNOPARAM("Accepnt Error: Player Max\n");
                pServer->Disconnect(newKey);
                break;
            }

            ZeroMemory(&exOver->mOver, sizeof(exOver->mOver));
            SOCKET cSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
            exOver->mWsaBuf.buf = reinterpret_cast<CHAR*>(cSocket);
            BOOL ret = AcceptEx(pServer->GetListenSocket(), cSocket
                , exOver->mMessageBuf
                , 0, 32, 32, NULL, &exOver->mOver);
            if (false == ret) {
                int err_num = WSAGetLastError();
                if (err_num != WSA_IO_PENDING)
                {
                    DEBUGMSGONEPARAM("Accept Error: %d\n", err_num);
                }
            }

            break;
        }
        case eOpType::OP_RECV: {
            unsigned char* packet_ptr = exOver->mMessageBuf;
            uint16_t BufSize = 0;
            std::memcpy(&BufSize, packet_ptr, sizeof(uint16_t));
            const int headerSize = sizeof(HEADER);
            int required_data = Transferred + pServer->GetSessions()[key]->GetPrevData();
            int packet_size = BufSize + headerSize;
            while (required_data >= packet_size) {
                if (required_data >= BUFSIZE) break;
                if (packet_size <= 0) break;    
                pPacketManager->ProcessPacket(packet_ptr, key);
                required_data -= packet_size;
                packet_ptr += packet_size;

                uint16_t nextBufSize = 0;
                std::memcpy(&nextBufSize, packet_ptr, sizeof(uint16_t));
                packet_size = nextBufSize + headerSize;
            }
            packet_size = 0;
            pServer->GetSessions()[key]->SetPrevData(0);
            if (0 != required_data)
                memcpy(exOver->mMessageBuf, packet_ptr, required_data);
            pServer->GetSessions()[key]->DoRecv();
            break;
        }
        case eOpType::OP_SEND: {
            delete exOver;
            break;
        }
        case eOpType::OP_EVENT:{
            EVENT* event = reinterpret_cast<EVENT*>(exOver->mMessageBuf);
            pPacketManager->ProcessEvent((eEventType)event->type, exOver->mMessageBuf);
            delete exOver;
            break;
        }
        }
    }
}
