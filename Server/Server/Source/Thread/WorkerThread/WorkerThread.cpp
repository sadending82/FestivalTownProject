#include "WorkerThread.h"
#include"../../Server/Server.h"

void WorkerThread::RunWorker()
{
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
            int newKey = m_pServer->SetKey();
            if (newKey != INVALIDKEY) {
                Session* newSession = m_pServer->GetSessions()[newKey];
                SOCKET cSocket = reinterpret_cast<SOCKET>(exOver->mWsaBuf.buf);
                newSession->SetSocket(cSocket);
                newSession->GetExOver().SetmOpType(eOpType::OP_RECV);
                newSession->SetPrevData(0);
                newSession->SetState(eSessionState::ST_ACCEPTED);
                newSession->SetID(newKey);
                CreateIoCompletionPort((HANDLE)newSession->GetSocket(), m_pServer->GetHcp(), newKey, 0);

                DEBUGMSGONEPARAM("Lobby Accept: %d\n", newKey);
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
            int headerSize = sizeof(HEADER);
            HEADER* header = reinterpret_cast<HEADER*>(packet_ptr);
            int required_data = Transferred + m_pServer->GetSessions()[key]->GetPrevData();
            int packet_size = header->size + headerSize;

            while (required_data >= packet_size) {
                if (required_data >= BUFSIZE) break;
                if (packet_size <= 0) break;
                unsigned char* data = packet_ptr + headerSize;
                m_pPacketManager->ProcessPacket(key, header->type, data, header->size);
                required_data -= packet_size;
                packet_ptr += packet_size;
                packet_size = packet_ptr[0];
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
        }

    }
}
