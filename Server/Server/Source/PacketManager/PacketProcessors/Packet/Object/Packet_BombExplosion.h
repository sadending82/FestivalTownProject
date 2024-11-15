#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::ObjectTable;

class Packet_BombExplosion : public PacketProcessor {

public:
	Packet_BombExplosion(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {
			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<BombExplosion>(nullptr)) {
				const BombExplosion* read = flatbuffers::GetRoot<BombExplosion>(data);

				int roomID = dynamic_cast<Player*>(pServer->GetSessions()[key])->GetRoomID();
				if (roomID == INVALIDKEY) {
					return;
				}
				int bombid = read->id();
				if (bombid <= INVALIDKEY) {
					return;
				}
				Room* room = pServer->GetRooms().at(roomID);
				if (room == nullptr) {
					return;
				}
				if (room->GetState() == eRoomState::RS_FREE) {
					return;
				}

				pPacketSender->SendBombExplosionPacket(roomID, bombid);
				room->DeleteBomb(bombid);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Packet_BombExplosion ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};