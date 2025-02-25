#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::GameTable;

class Packet_GameReady : public PacketProcessor {

public:
	Packet_GameReady(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) { 
		try {
			const GameReady* read = flatbuffers::GetRoot<GameReady>(data);
			Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
			if (player == nullptr) {
				return;
			}
			int roomID = player->GetRoomID();
			if (roomID == INVALIDKEY) {
				return;
			}
			Room* room = pServer->GetRooms().at(roomID);
			if (room == nullptr) {
				return;
			}

			if (player->GetSessionState() != eSessionState::ST_GAMELOADING) {
				return;
			}

			room->SetIsPlayerReady(player->GetInGameID(), true);
			pServer->GetLobbyManager()->CheckReadyToGamePlay(room, roomID);
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_GameReady ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};