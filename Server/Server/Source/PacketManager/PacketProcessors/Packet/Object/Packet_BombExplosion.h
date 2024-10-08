#pragma once
#include "../../PacketProcessor.h"

using namespace PacketTable::ObjectTable;

class Packet_BombExplosion : public PacketProcessor {

public:
	Packet_BombExplosion(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(const uint8_t* data, const int size, const int key) {
		try {

			mBuilder.Clear();

			flatbuffers::Verifier verifier(data, size);
			if (verifier.VerifyBuffer<BombExplosion>(nullptr)) {
				const BombExplosion* read = flatbuffers::GetRoot<BombExplosion>(data);

				int roomid = dynamic_cast<Player*>(pServer->GetSessions()[key])->GetRoomID();
				int bombid = read->id();
				if (bombid <= INVALIDKEY) {
					return;
				}
				Room* room = pServer->GetRooms().at(roomid);
				if (room == nullptr) {
					return;
				}
				if (room->GetState() == eRoomState::RS_FREE) {
					return;
				}

				pPacketSender->SendBombExplosionPacket(roomid, bombid);
				room->DeleteBomb(bombid);
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[ERROR] : " << e.what() << " KEY : " << key << std::endl;
		}
	}

private:
};