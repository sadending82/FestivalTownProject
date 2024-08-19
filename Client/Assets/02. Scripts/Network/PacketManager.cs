using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using UnityEngine;
using NetworkProtocol;
using Network.PacketProcessor;
using UnityEngine.UIElements;


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
            { ePacketType.S2C_HEARTBEAT, new HeartBeatProcessor() },


            { ePacketType.S2C_PLAYERADD, new PlayerAddProcessor() },
            { ePacketType.S2C_GAMEINFO, new GameInfoProcessor() },
            { ePacketType.S2C_PLAYERMOVE, new PlayerMoveProcessor() },
            { ePacketType.S2C_PLAYERSTOP, new PlayerStopProcessor() },
            { ePacketType.S2C_PLAYERPOSSYNC, new PlayerPosSyncProcessor() },
            { ePacketType.S2C_PLAYERGRABBOMB, new PlayerGrabBombProcessor() },
            { ePacketType.S2C_PLAYERTHROWBOMB, new PlayerThrowBombProcessor() },


            { ePacketType.S2C_BLOCKDROP, new BlockDropProcessor() },
            { ePacketType.S2C_BOMBSPAWN, new BombSpawnProcessor() },
            { ePacketType.S2C_BOMBEXPLOSION, new BombExplosionProcessor() },
            { ePacketType.S2C_BOMBPOSSYNC, new BombPositionSyncProcessor() },


            { ePacketType.S2C_LIFEREDUCE, new LifeReduceProcessor() },
            { ePacketType.S2C_REMAINTIMESYNC, new RemainTimeSyncProcessor() }
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

    public void SendPlayerMovePacket(Vector3 position, Vector3 direction, int id, ePlayerState state)
    {

        byte[] packet = _packetMaker.MakePlayerMovePacket(position, direction, id, state);
        if (packet == null) { return; }
        SendPacket(packet);
    }

    public void SendPlayerStopPacket(Vector3 position, Vector3 direction, int id, ePlayerState state)
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
}
