#include "WorkerThread.h"
#include "../../Server/Server.h"
#include "../../PacketManager/PacketManager.h"
#include <mstcpip.h>

WorkerThread::~WorkerThread()
{

}

void WorkerThread::RunWorker()
{
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
                newSession->SessionInit(cSocket, newKey);

                //// Keep-Alive 활성화
                //BOOL keepAlive = TRUE;
                //setsockopt(cSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&keepAlive, sizeof(keepAlive));

                //// 상세 Keep-Alive 설정
                //tcp_keepalive kaSettings;
                //kaSettings.onoff = 1;
                //kaSettings.keepalivetime = 5000;
                //kaSettings.keepaliveinterval = 5000;

                //DWORD bytesReturned;
                //WSAIoctl(cSocket, SIO_KEEPALIVE_VALS, &kaSettings, sizeof(kaSettings), NULL, 0, &bytesReturned, NULL, NULL);

                CreateIoCompletionPort((HANDLE)newSession->GetSocket(), pServer->GetHcp(), newKey, 0);
                DEBUGMSGONEPARAM("Lobby Accept: %d\n", newKey);

                // 접속하자 마자 HeartBeat 이벤트 추가
                pServer->StartHeartBeat(newKey, newSession->GetAuthenticationKey());

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
            Session* session = pServer->GetSessions()[key];

            session->SetIsHeartbeatAck(true);

            unsigned char* packet_ptr = exOver->GetMessageBuf();

            // 이전 작업에서 남은 데이터와 현재 받은 데이터를 재조립한 후 데이터 크기
            int pendingData = Transferred + session->GetPrevDataSize();

            // 헤더의 버퍼 크기 정보만 읽어서 packetSize 확인
            uint16_t BufSize = 0;
            std::memcpy(&BufSize, packet_ptr, sizeof(uint16_t));
            const int headerSize = sizeof(HEADER);
            int packetSize = BufSize + headerSize;

            // 헤더의 정보를 바탕으로 순차적으로 데이터 처리
            while (pendingData >= packetSize) {
                if (pendingData >= BUFSIZE) break;
                if (packetSize <= 0) break;
      
                pPacketManager->ProcessPacket(packet_ptr, key);
                pendingData -= packetSize;
                packet_ptr += packetSize;

                // 처리 후 남은 데이터의 헤더를 읽어 packetSize 갱신
                uint16_t nextBufSize = 0;
                std::memcpy(&nextBufSize, packet_ptr, sizeof(uint16_t));
                packetSize = nextBufSize + headerSize;
            }
            session->SetPrevDataSize(pendingData);
            if (0 != pendingData) {
                memcpy(exOver->mMessageBuf, packet_ptr, pendingData);
            }
            session->DoRecv();
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
