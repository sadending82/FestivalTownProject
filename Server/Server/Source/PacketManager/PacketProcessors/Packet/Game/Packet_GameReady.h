#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::GameTable;

class Packet_GameReady : public PacketProcessor {

public:
	Packet_GameReady(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) { 
		try {

			mBuilder.Clear();
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<GameReady>(nullptr)) {

				const GameReady* read = flatbuffers::GetRoot<GameReady>(data);
				Player* player = dynamic_cast<Player*>(pServer->GetSessions()[key]);
				if (player == nullptr) {
					return;
				}
				int roomid = player->GetRoomID();
				Room* room = pServer->GetRooms().at(roomid);
				if (room == nullptr) {
					return;
				}

				room->AddReadyCnt();

				SERVER_MODE serverMode = pServer->GetMode();
				switch (serverMode) {
				case SERVER_MODE::LIVE: {
					if (room->GetReadyCnt() == room->GetPlayerCnt()) {

						if (room->SetAllPlayerReady(true) == true) {

							pPacketSender->SendAllPlayerReady(roomid);
							PushEventGameStart(pServer->GetTimer(), roomid, room->GetRoomCode());
						}
					}
				}break;
				case SERVER_MODE::TEST: {
					pPacketSender->SendAllPlayerReady(roomid);
					PushEventGameStart(pServer->GetTimer(), roomid, pServer->GetRooms()[roomid]->GetRoomCode());
				}break;
				}
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_GameReady ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};