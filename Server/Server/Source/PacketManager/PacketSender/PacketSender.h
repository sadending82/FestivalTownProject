#pragma once
#include "../PacketMaker/PacketMaker.h"

class PacketSender
{
public:
	PacketSender(class Server* server, class PacketMaker* packetMaker) :mServer(server), mPacketMaker(packetMaker) {}
	~PacketSender();

	void SendLoginResponse(int sessionID, int result, int uid, std::string& nickname, int point, int gold);
	void SendSignUpResponse(int sessionID, int result);

	void SendPlayerAdd(int roomID);
	void SendPlayerDelete(int roomID, int inGameID);
	void SendGameMatchingResponse(int sessionID);
	void SendGameStart(int roomID);
	void SendAllPlayerReady(int roomID);

	void SendHeartBeatPacket(int sessionID);
	void SendBlockDropPacket(int roomID, int spawnCount, int blockType);
	void SendBombSpawnPacket(std::vector<Vector3f>& Positions, std::vector<int>& bombIDs, int explosionInterval, int roomID);
	void SendBombExplosionPacket(int roomID, int bombID);
	void SendLifeReducePacket(int team, int lifeCount, int roomID);
	void SendRemainTimeSync(int roomID);

	void SendGameStartPacket(int roomID);
	void SendGameEndPacket(int roomID, uint8_t winningTeams_flag);
	void SendGameResultPacket(int roomID, std::set<int>& winningTeams);

	void SendGameHostChange(int sessionID);
	void SendPlayerDeadPacket(int inGameID, int roomID, int spawn_delay);
	void SendPlayerRespawn(int inGameID, int roomID);
	void SendWeaponSpawnPacket(std::vector<Vector3f>& positions, std::vector<int>& weaponIDs, std::vector<int>& weaponTypes, int roomID);
	void SendWeaponDropPacket(Vector3f position, int roomID, int weaponID);
	void SendPlayerCalculatedDamage(int targetID, int roomID, int attackType, int hp, int damageAmount, int targetStaminaLoss, Vector3f knockback_direction);
	void SendPlayerGroggyPacket(int inGameID, int roomID);
	void SendPlayerGroggyRecoveryPacket(int inGameID, int roomID, int recoveredStamina);
	void SendPlayerThrowOtherPlayerPacket(int roomID, int inGameID, Vector3f myPosition, Vector3f myDirection, int targetID, Vector3f targetPosition, Vector3f targetDirection);

private:
	class Server* mServer = nullptr;
	class PacketMaker* mPacketMaker = nullptr;
};