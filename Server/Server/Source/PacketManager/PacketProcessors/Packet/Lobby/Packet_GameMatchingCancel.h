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

				const GameMatchingCancel* read = flatbuffers::GetRoot<GameMatchingCancel>(data);

				Session* session = pServer->GetSessions()[key];

				MatchMakingManager* MatchMakingManager = pServer->GetMatchMakingManager();

				session->GetSessionStateLock().lock();
				session->SetSessionState(eSessionState::ST_ACCEPTED);

				MatchMakingManager->GetMatchingLock().lock();
				MatchMakingManager->GetMatchingQueue(eMatchingType::FITH_TEAM).erase(dynamic_cast<Player*>(session));
				MatchMakingManager->GetMatchingLock().unlock();

				session->GetSessionStateLock().unlock();

				std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_MATCHING_CANCEL, data, size);
				session->DoSend(send_buffer.data(), send_buffer.size());
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_GameMatchingCancel ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};