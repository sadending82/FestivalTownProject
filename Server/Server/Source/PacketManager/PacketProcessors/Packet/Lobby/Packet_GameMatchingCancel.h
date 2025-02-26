#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::LobbyTable;

class Packet_GameMatchingCancel : public PacketProcessor {

public:
	Packet_GameMatchingCancel(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) { 
		try {
			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (pServer->GetMode() == SERVER_MODE::LIVE && player->GetIsAuthenticated() == false) {
				pServer->Disconnect(key);
				return;
			}

			const GameMatchingCancel* read = flatbuffers::GetRoot<GameMatchingCancel>(data);

			eMatchingType matchingType = player->GetMatchingRequestType();

			MatchMakingManager* MatchMakingManager = pServer->GetMatchMakingManager();

			{
				MatchMakingManager->GetMatchingLock().lock();

				MATCHING_QUEUE& matchingQueue = MatchMakingManager->GetMatchingQueue(matchingType);

				if (matchingQueue.empty() == true) {
					MatchMakingManager->GetMatchingLock().unlock();
					return;
				}

				int top_ID = matchingQueue.begin()->first;
				long long top_requestTime = matchingQueue.begin()->second;

				matchingQueue.erase({ key, player->GetMatchingRequestTime() });

				// 최장 대기 유저가 매칭 취소 시 그 다음 최장 대기 유저 기준으로 매칭 시퀀스 갱신
				if (top_ID == key && player->GetMatchingRequestTime() == top_requestTime) {
					MatchMakingManager->SetMatchingSequence(matchingType, eMatchingSequence::MS_None);
					MatchMakingManager->UpdateMatchingSequence(matchingType);
				}

				player->SetMatchingRequestTime(0);

				std::wcout << L"Nickname: " << player->GetNickName() << L" Matching Cancel / Match: " << matchingType << L" / wating Player - " << MatchMakingManager->GetMatchingQueue(matchingType).size() << std::endl;
				
				MatchMakingManager->GetMatchingLock().unlock();
			}

			if (player->ChangeSessionState(eSessionState::ST_MATCHWAITING, eSessionState::ST_ACCEPTED) == false) {
				DEBUGMSGNOPARAM("Error!: Fail Change Session State ST_MATCHWAITING To ST_ACCEPTED\n");
			}

			std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_MATCHING_CANCEL, data, size);
			player->DoSend(send_buffer.data(), send_buffer.size());
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_GameMatchingCancel ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};