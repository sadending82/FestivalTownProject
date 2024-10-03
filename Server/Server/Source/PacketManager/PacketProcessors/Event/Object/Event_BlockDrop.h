#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

using namespace PacketTable::ObjectTable;

class Event_BlockDrop : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) { 
		try {
			EV_OBJECT_DROP* event = reinterpret_cast<EV_OBJECT_DROP*>(buf);

			int roomid = event->roomID;
			Room* room = pServer->GetRooms().at(roomid);
			if (room == nullptr) {
				return;
			}
			long long roomCode = room->GetRoomCode();
			if (roomCode != event->roomCode) {
				return;
			}
			if (room->GetState() == eRoomState::RS_FREE) {
				return;
			}

			GameMode gameMode = room->GetGameMode();

			GameModeInfo& modeInfo = pServer->GetTableManager()->GetGameModeData()[gameMode];

			int blockType = event->objectType;
			int nextEventTime = 0;
			int spawnCnt = 0;

			switch (blockType) {
			case eBlockType::BT_BLOCK_2_2_2: {
				nextEventTime = modeInfo.Block1_Spawn_Time; // seconds
				spawnCnt = modeInfo.Block1_Spawn_Count;
			}
										   break;
			case eBlockType::BT_BLOCK_2_2_1: {
				nextEventTime = modeInfo.Block2_Spawn_Time; // seconds
				spawnCnt = modeInfo.Block2_Spawn_Count;
			}
										   break;
			default:
			{
				return;
			}
			}
			PushEventBlockDrop(pServer->GetTimer(), event->roomID, event->roomCode, blockType, nextEventTime);

			pServer->GetPacketSender()->SendBlockDropPacket(event->roomID, spawnCnt, blockType);
		}
		catch (const std::exception& e) {
			std::cerr << "[ERROR] : " << e.what() << std::endl;
		}
	}

private:
	flatbuffers::FlatBufferBuilder mBuilder;
};