#pragma once
#include "../PacketMaker/PacketMaker.h"

class PacketSender
{
public:
	PacketSender(class Server* server, class PacketMaker* packetMaker) :mServer(server), mPacketMaker(packetMaker) {}
	~PacketSender();

	void SendPlayerAdd(int roomID);
	void SendPlayerDelete(int roomID, int inGameID);
	void SendGameMatchingResponse(int sessionID);
	void SendGameStart(int roomID);
	void SendAllPlayerReady(int roomID);

	void SendHeartBeatPacket(int sessionID);
	void SendBlockDropPacket(int roomID, int spawnCount);
	void SendBombSpawnPacket(int roomID, int spawnCount);
	void SendBombExplosionPacket(int roomID, int bombID);
	void SendLifeReducePacket(int team, int lifeCount, int roomID);
	void SendRemainTimeSync(int roomID);

	void SendGameStartPacket(int roomID);
	void SendGameEndPacket(int roomID, uint8_t winningTeams_flag);
	void SendGameResultPacket(int roomID, std::vector<int>& winningTeams);

	void SendGameHostChange(int sessionID);
	void SendPlayerDeadPacket(int inGameID, int roomID);
	void SendPlayerRespawn(int inGameID, int roomID);
	void SendWeaponSpawnPacket(int roomID, int spawnCount);
	void SendWeaponDropPacket(Vector3f position, int roomID, int weaponID);
	void SendPlayerCalculatedDamage(int targetID, int roomID, int attackType, int hp, int damageAmount, Vector3f knockback_direction);

private:
	class Server* mServer = nullptr;
	class PacketMaker* mPacketMaker = nullptr;
};