#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::LobbyTable;

class Packet_GameMatchingCancel : public PacketProcessor {

public:
	Packet_GameMatchingCancel(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) { 
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<GameMatchingCancel>(nullptr)) {

				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				if (pServer->GetMode() == SERVER_MODE::LIVE) {
					if (player->GetIsAuthenticated() == false) {
						pServer->Disconnect(key);
						return;
					}
				}

				const GameMatchingCancel* read = flatbuffers::GetRoot<GameMatchingCancel>(data);

				MatchMakingManager* MatchMakingManager = pServer->GetMatchMakingManager();

				MatchMakingManager->GetMatchingLock().lock();

				eMatchingType matchingType = player->GetMatchingRequestType();

				MATCHING_QUEUE& matchingQueue = MatchMakingManager->GetMatchingQueue(matchingType);

				if (matchingQueue.empty() == true) {
					MatchMakingManager->GetMatchingLock().unlock();
					return;
				}

				int top_ID = matchingQueue.begin()->first;
				long long top_requestTime = matchingQueue.begin()->second;

				matchingQueue.erase({key, player->GetMatchingRequestTime()});

				// ���� ��� ������ ��Ī ��� �� �� ���� ���� ��� ���� �������� ��Ī ������ ����
				if (top_ID == key && player->GetMatchingRequestTime() == top_requestTime) {
					MatchMakingManager->SetMatchingSequence(matchingType, eMatchingSequence::MS_None);
					MatchMakingManager->UpdateMatchingSequence(matchingType);
				}

				player->SetMatchingRequestTime(0);

				std::wcout << L"Nickname: " << player->GetNickName() << L" Matching Cancel / Match: " << matchingType << L" / wating Player - " << MatchMakingManager->GetMatchingQueue(matchingType).size() << std::endl;

				MatchMakingManager->GetMatchingLock().unlock();

				player->GetSessionStateLock().lock();
				if (player->GetSessionState() == eSessionState::ST_MATCHWAITING) {
					player->SetSessionState(eSessionState::ST_ACCEPTED);
				}
				player->GetSessionStateLock().unlock();

				std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_MATCHING_CANCEL, data, size);
				player->DoSend(send_buffer.data(), send_buffer.size());
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_GameMatchingCancel ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};