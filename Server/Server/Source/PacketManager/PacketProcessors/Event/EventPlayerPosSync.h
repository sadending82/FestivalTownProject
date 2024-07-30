#pragma once
#include "../PacketProcessor.h"
#include "../../../Event/Event.h"

using namespace PacketTable::PlayerTable;

class EventPlayerPosSync : public PacketProcessor {

public:

	virtual void Process(Server* pServer, const char* buf) {
		// 방이 활성화 되어있는지 확인해야함
		EV_PLAYER_POS_SYNC* event = reinterpret_cast<EV_PLAYER_POS_SYNC*>(&buf);

		int roomID = event->roodID;

		Room* room = pServer->GetRooms()[roomID];

		for (Player* player : room->GetPlayerList()) {
			if (player == nullptr) continue;
			mBuilder.Clear();
			int inGameID = player->GetInGameID();
			auto pos = CreateVec3(mBuilder, player->GetPosition().x, player->GetPosition().y, player->GetPosition().z);
			auto dir = CreateVec3(mBuilder, player->GetDirection().x, player->GetDirection().y, player->GetDirection().z);

			mBuilder.Finish(PacketTable::PlayerTable::CreatePlayerPos(mBuilder, inGameID, pos, dir));
			std::vector<uint8_t> send_buffer = MakeBuffer(ePacketType::S2C_PLAYERADD, mBuilder.GetBufferPointer(), mBuilder.GetSize());

			pServer->SendAllPlayerInRoom(send_buffer.data(), send_buffer.size(), roomID);
		}

		PushEventPlayerPosSync(pServer->GetTimer(), roomID);
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};