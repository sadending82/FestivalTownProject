#pragma once
#include "../../PacketProcessor.h"
#include "../../../../Event/Event.h"

using namespace PacketTable::ObjectTable;

class Event_WeaponSpawn : public PacketProcessor {

public:
	Event_WeaponSpawn(Server* server, PacketSender* packetSender) : PacketProcessor(server, packetSender) {}

	virtual void Process(unsigned char* buf) { 
		try {
			EV_WEAPON_SPAWN* event = reinterpret_cast<EV_WEAPON_SPAWN*>(buf);

			int roomID = event->roomID;
			Room* room = pServer->GetRooms().at(roomID);
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
			GameModeData& modeInfo = room->GetGameModeData();
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

			pServer->GetGameManagers()[gameMode]->WeaponSpawn(room, event->roomID, (eWeaponType)event->weaponType, spawnCnt);
			PushEventWeaponSpawn(pServer->GetTimer(), event->roomID, event->roomCode, event->weaponType, nextEventTime);
		}
		catch (const std::exception& e) {
			std::cerr << "[Event_WeaponSpawn ERROR] : " << e.what() << std::endl;
		}
	}

private:
};