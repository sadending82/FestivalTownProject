#include "WorkerThread.h"
#include"../../Server/Server.h"

void WorkerThread::RunWorker()
{
    m_pPacketManager = new PacketManager();
    m_pPacketManager->Init(m_pServer);

    while (1) {
        DWORD Transferred;
        ULONG key;
        WSAOVERLAPPED* over;

        BOOL retval = GetQueuedCompletionStatus(m_pServer->GetHcp(), &Transferred,
            (PULONG_PTR)&key, &over, INFINITE);

        ExOver* exOver = reinterpret_cast<ExOver*>(over);

        if (false == retval && Transferred == 0) {
            m_pServer->Disconnect(key);
        }

        switch (exOver->mOpType) {
        case eOpType::OP_ACCEPT: {
            int newKey = m_pServer->SetSessionID();
            if (newKey != INVALIDKEY) {
                Session* newSession = m_pServer->GetSessions()[newKey];
                SOCKET cSocket = reinterpret_cast<SOCKET>(exOver->mWsaBuf.buf);
                newSession->SetSocket(cSocket);
                newSession->GetExOver().SetmOpType(eOpType::OP_RECV);
                newSession->SetPrevData(0);
                newSession->SetSessionID(newKey);

                CreateIoCompletionPort((HANDLE)newSession->GetSocket(), m_pServer->GetHcp(), newKey, 0);
                DEBUGMSGONEPARAM("Lobby Accept: %d\n", newKey);

                // 접속하자 마자 HeartBeat 이벤트 추가
                m_pServer->StartHeartBeat(newKey);

                newSession->DoRecv();
            }
            else {
                DEBUGMSGNOPARAM("Accepnt Error: Player Max\n");
                m_pServer->Disconnect(newKey);
                break;
            }

            ZeroMemory(&exOver->mOver, sizeof(exOver->mOver));
            SOCKET cSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
            exOver->mWsaBuf.buf = reinterpret_cast<CHAR*>(cSocket);
            BOOL ret = AcceptEx(m_pServer->GetListenSocket(), cSocket
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
            int required_data = Transferred + m_pServer->GetSessions()[key]->GetPrevData();
            int packet_size = BufSize + headerSize;
            while (required_data >= packet_size) {
                if (required_data >= BUFSIZE) break;
                if (packet_size <= 0) break;
                m_pPacketManager->ProcessPacket(packet_ptr, key);
                required_data -= packet_size;
                packet_ptr += packet_size;

                uint16_t nextBufSize = 0;
                std::memcpy(&nextBufSize, packet_ptr, sizeof(uint16_t));
                packet_size = nextBufSize + headerSize;
            }
            packet_size = 0;
            m_pServer->GetSessions()[key]->SetPrevData(0);
            if (0 != required_data)
                memcpy(exOver->mMessageBuf, packet_ptr, required_data);
            m_pServer->GetSessions()[key]->DoRecv();
            break;
        }
        case eOpType::OP_SEND: {
            delete exOver;
            break;
        }
        case eOpType::OP_EVENT:{
            EVENT* event = reinterpret_cast<EVENT*>(exOver->mMessageBuf);
            m_pPacketManager->ProcessEvent((eEventType)event->type, exOver->mMessageBuf);
            break;
        }
        }

    }
}
