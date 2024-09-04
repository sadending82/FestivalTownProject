using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using UnityEngine;
using NetworkProtocol;
using ClientProtocol;
using Network.PacketProcessor;
using UnityEngine.UIElements;
using PacketTable.GameTable;

public class PacketManager : MonoBehaviour 
{
    private Dictionary<ePacketType, PacketProcessor> processorDict { get; set; }

    public PacketProcessor GetProcessor(ePacketType type)
    {
        return processorDict[type];
    }

    private TcpClient _connection;
    private PacketMaker _packetMaker = new PacketMaker();

    public PacketManager()
    {
        _connection = null;
    }

    public PacketManager(TcpClient connection)
    {
        _connection = connection;
    }

    public void SetConnection(TcpClient connection)
    {
        _connection = connection;
    }

    public void Init()
    {
        processorDict = new Dictionary<ePacketType, PacketProcessor>
        {
            { ePacketType.S2C_HEART_BEAT, new HeartBeatProcessor() },


            { ePacketType.S2C_PLAYER_ADD, new PlayerAddProcessor() },
            { ePacketType.S2C_PLAYER_MOVE, new PlayerMoveProcessor() },
            { ePacketType.S2C_PLAYER_STOP, new PlayerStopProcessor() },
            { ePacketType.S2C_PLAYER_POS_SYNC, new PlayerPosSyncProcessor() },
            { ePacketType.S2C_PLAYER_GRAB_BOMB, new PlayerGrabBombProcessor() },
            { ePacketType.S2C_PLAYER_THROW_BOMB, new PlayerThrowBombProcessor() },
            { ePacketType.S2C_PLAYER_ANIMATION, new PlayerAnimationProcessor() },
            { ePacketType.S2C_PLAYER_CALCULATED_DAMAGE, new PlayerCalculatedDamageProcessor() },
            { ePacketType.S2C_PLAYER_DEAD, new PlayerDeadProcessor() },
            { ePacketType.S2C_PLAYER_RESPAWN, new PlayerRespawnProcessor() },


            { ePacketType.S2C_BLOCK_DROP, new BlockDropProcessor() },
            { ePacketType.S2C_BOMB_SPAWN, new BombSpawnProcessor() },
            { ePacketType.S2C_BOMB_EXPLOSION, new BombExplosionProcessor() },
            { ePacketType.S2C_BOMB_POS_SYNC, new BombPositionSyncProcessor() },
            { ePacketType.S2C_WEAPON_SPAWN, new WeaponSpawnProcessor() },

            { ePacketType.S2C_LIFE_REDUCE, new LifeReduceProcessor() },
            { ePacketType.S2C_REMAIN_TIME_SYNC, new RemainTimeSyncProcessor() },

            { ePacketType.S2C_MATCHING_RESPONSE, new GameMatchingResponseProcessor() },
            { ePacketType.S2C_ALL_PLAYER_READY, new AllPlayerReadyProcessor() },
            { ePacketType.S2C_GAME_START, new GameStartProcessor() },
            { ePacketType.S2C_GAME_END, new GameEndProcessor() },
            { ePacketType.S2C_GAME_HOST_CHANGE, new GameHostChangeProcessor() }
        };
    }

    public void SendPacket(Byte[] buffer)
    {
        if (_connection == null)
        {
            return;
        }
        try
        {
            NetworkStream stream = _connection.GetStream();
            if (stream.CanWrite)
            {
                stream.Write(buffer, 0, buffer.Length);
            }
        }
        catch (SocketException Exception)
        {
            Debug.Log("Write exception: " + Exception);
        }
    }

    private byte[] Serialize<T>(T packet)
    {
        var buffer = new byte[Marshal.SizeOf(typeof(T))];

        var gch = GCHandle.Alloc(buffer, GCHandleType.Pinned);
        var pBuffer = gch.AddrOfPinnedObject();

        Marshal.StructureToPtr(packet, pBuffer, false);
        gch.Free();

        //Debug.Log("Serializing.");

        return buffer;
    }

    public void SendPlayerMovePacket(Vector3 position, Vector3 direction, int id, ePlayerMoveState state)
    {

        byte[] packet = _packetMaker.MakePlayerMovePacket(position, direction, id, state);
        if (packet == null) { return; }
        SendPacket(packet);
    }

    public void SendPlayerStopPacket(Vector3 position, Vector3 direction, int id, ePlayerMoveState state)
    {

        byte[] packet = _packetMaker.MakePlayerStopPacket(position, direction, id, state);
        if (packet == null) { return; }
        SendPacket(packet);
    }

    public void SendPlayerPosPacket(Vector3 position, Vector3 direction, int id)
    {
        byte[] packet = _packetMaker.MakePlayerPosSyncPacket(position, direction, id);
        if (packet == null) { return; }
        SendPacket(packet);
    }

    public void SendHeartBeatPacket()
    {
        byte[] packet = _packetMaker.MakeHeartBeatPacket();
        if (packet == null) { return; }
        SendPacket(packet);
    }

    public void SendBombInputPacket(int bombid, int team, int playerid = 0)
    {
        byte[] packet = _packetMaker.MakeBombInputPacket(playerid, bombid, team);
        if (packet == null) { return; }
        SendPacket(packet);
    }

    public void SendPlayerGrabBombPacket(Vector3 position, Vector3 direction, int playerID, int BombID)
    {
        byte[] packet = _packetMaker.MakePlayerGrabBombPacket(position, direction, playerID, BombID);
        if (packet == null) { return; }
        SendPacket(packet);
    }

    public void SendPlayerThrowBombPacket(Vector3 position, Vector3 direction, int playerID, int BombID)
    {
        byte[] packet = _packetMaker.MakePlayerThrowBombPacket(position, direction, playerID, BombID);
        if (packet == null) { return; }
        SendPacket(packet);
    }

    public void SendPlayerAnimationPacket(Vector3 position, Vector3 direction, int playerID, UpperBodyAnimationState animation)
    {
        byte[] packet = _packetMaker.MakePlayerAnimationPacket(position, direction, playerID, (int)animation);
        if (packet == null) { return; }
        SendPacket(packet);
    }

    public void SendPlayerDamageReceivePacket(int attackerID, int targetID, int weapon, eAttackType attackType, Vector3 knockback_direction)
    {
        byte[] packet = _packetMaker.MakePlayerDamageReceivePacket(attackerID, targetID, weapon, (int)attackType, knockback_direction);
        if (packet == null) { return; }
        SendPacket(packet);
    }

    public void SendBombPositionSyncPacket(Vector3 position, int BombID)
    {
        byte[] packet = _packetMaker.MakeBombPositionSyncPacket(position, BombID);
        if (packet == null) { return; }
        SendPacket(packet);
    }

    public void SendBombExplosionPacket(Vector3 position, int BombID)
    {
        byte[] packet = _packetMaker.MakeBombExplosionPacket(position, BombID);
        if (packet == null) { return; }
        SendPacket(packet);
    }

    public void SendGameMatchingRequest()
    {
        byte[] packet = _packetMaker.MakeGameMatchingRequestPacket();
        if (packet == null) { return; }
        SendPacket(packet);
    }

    public void SendGameReady()
    {
        byte[] packet = _packetMaker.MakeGameReadyPacket();
        if (packet == null) { return; }
        SendPacket(packet);
    }
}
