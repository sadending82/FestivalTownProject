#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

using namespace PacketTable::ObjectTable;

class Event_WeaponSpawn : public PacketProcessor {

public:

	virtual void Process(Server* pServer, unsigned char* buf) {
		EV_WEAPON_SPAWN* event = reinterpret_cast<EV_WEAPON_SPAWN*>(buf);

		int roomid = event->roomID;
		Room* room = pServer->GetRooms()[roomid];
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
		int weaponType = event->weaponType;
		int nextEventTime = 0;
		int spawnCnt = 0;
		
		switch (weaponType) {
		case eWeaponType::WT_FRYING_PAN: {
			nextEventTime = modeInfo.Weapon1_Spawn_Time; // seconds
			spawnCnt = modeInfo.Weapon1_Spawn_Count;
		}
									   break;
		case eWeaponType::WT_BAT: {
			nextEventTime = modeInfo.Weapon2_Spawn_Time; // seconds
			spawnCnt = modeInfo.Weapon2_Spawn_Count;
		}
								break;
		default:
		{
			return;
		}
		}

		PushEventWeaponSpawn(pServer->GetTimer(), event->roomID, event->roomCode, event->weaponType, nextEventTime);
		pServer->GetPacketSender()->SendWeaponSpawnPacket(roomid, spawnCnt, weaponType);
	}

private:

	flatbuffers::FlatBufferBuilder mBuilder;
};