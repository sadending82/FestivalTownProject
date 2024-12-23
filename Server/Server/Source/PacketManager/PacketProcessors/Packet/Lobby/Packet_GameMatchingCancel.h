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

				player->GetSessionStateLock().lock();

				MatchMakingManager->GetMatchingLock().lock();

				eMatchingType matchingType = player->GetMatchingRequestType();

				MATCHING_QUEUE& matchingQueue = MatchMakingManager->GetMatchingQueue(matchingType);

				if (matchingQueue.empty() == true) {
					return;
				}

				int top_ID = matchingQueue.begin()->first;
				long long top_requestTime = matchingQueue.begin()->second;

				matchingQueue.erase({key, player->GetMatchingRequestTime()});

				// 최장 대기 유저가 매칭 취소 시 그 다음 최장 대기 유저 기준으로 매칭 시퀀스 갱신
				if (top_ID == key && player->GetMatchingRequestTime() == top_requestTime) {
					MatchMakingManager->SetMatchingSequence(matchingType, eMatchingSequence::MS_None);
					MatchMakingManager->UpdateMatchingSequence(matchingType);
				}

				player->SetMatchingRequestTime(0);

				MatchMakingManager->GetMatchingLock().unlock();

				player->SetSessionState(eSessionState::ST_ACCEPTED);
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