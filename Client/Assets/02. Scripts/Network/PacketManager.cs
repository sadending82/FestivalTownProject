using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using UnityEngine;
using NetworkProtocol;
using Google.FlatBuffers;
using PacketTable.PlayerTable;
using Network.PacketProcessor;
using UnityEngine.UIElements;
using PacketTable.UtilitiesTable;

public class PacketManager : MonoBehaviour 
{
    private Dictionary<ePacketType, PacketProcessor> processorDict { get; set; }

    public PacketProcessor GetProcessor(ePacketType type)
    {
        return processorDict[type];
    }

    private TcpClient _connection;

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
            { ePacketType.S2C_PLAYERGAMEINFO, new PlayerGameInfoProcessor() },
            { ePacketType.S2C_PLAYERMOVE, new PlayerMoveProcessor() },
            { ePacketType.S2C_PLAYERSTOP, new PlayerStopProcessor() },
            { ePacketType.S2C_PLAYERPOSSYNC, new PlayerPosSyncProcessor() }
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

    public byte[] CreatePlayerMovePacket(Vector3 position, Vector3 direction, int id, ePlayerState state)
    {
        var builder = new FlatBufferBuilder(1);
        var pos = Vec3.CreateVec3(builder, position.x, position.y, position.z);
        var dir = Vec3.CreateVec3(builder, direction.x, direction.y, direction.z);
        PlayerMove.StartPlayerMove(builder);
        PlayerMove.AddPos(builder, pos);
        PlayerMove.AddDirection(builder, dir);
        PlayerMove.AddId(builder, id);
        PlayerMove.AddState(builder, (int)state);
        var offset = PlayerMove.EndPlayerMove(builder);
        builder.Finish(offset.Value);

        //var buf = builder.DataBuffer;
        //var verifier = new Verifier(buf);
        //if (PlayerMoveVerify.Verify(verifier, (uint)offset.Value) == false)
        //{
        //    Debug.Log("invaild buf / CreatePlayerMovePacket");
        //    return null;
        //}

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYERMOVE, flatBufferSize = (ushort)data.Length };

        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] CreatePlayerStopPacket(Vector3 position, Vector3 direction, int id, ePlayerState state)
    {
        var builder = new FlatBufferBuilder(1);
        var pos = Vec3.CreateVec3(builder, position.x, position.y, position.z);
        var dir = Vec3.CreateVec3(builder, direction.x, direction.y, direction.z);
        PlayerStop.StartPlayerStop(builder);
        PlayerStop.AddPos(builder, pos);
        PlayerStop.AddDirection(builder, dir);
        PlayerStop.AddId(builder, id);
        PlayerStop.AddState(builder, (int)state);
        var offset = PlayerStop.EndPlayerStop(builder);
        builder.Finish(offset.Value);

        //var buf = builder.DataBuffer;
        //var verifier = new Verifier(buf);
        //if (PlayerStopVerify.Verify(verifier, (uint)offset.Value) == false)
        //{
        //    Debug.Log("invaild buf / CreatePlayerStopPacket");
        //    return null;
        //}

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYERSTOP, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] CreatePlayerPosSyncPacket(Vector3 position, Vector3 direction, int id)
    {
        var builder = new FlatBufferBuilder(1);
        var pos = Vec3.CreateVec3(builder, position.x, position.y, position.z);
        var dir = Vec3.CreateVec3(builder, direction.x, direction.y, direction.z);
        PlayerPos.StartPlayerPos(builder);
        PlayerPos.AddPos(builder, pos);
        PlayerPos.AddDirection(builder, dir);
        PlayerPos.AddId(builder, id);
        var offset = PlayerPos.EndPlayerPos(builder);
        builder.Finish(offset.Value);

        //var buf = builder.DataBuffer;
        //var verifier = new Verifier(buf);
        //if (PlayerPosSyncVerify.Verify(verifier, (uint)offset.Value) == false)
        //{
        //    Debug.Log("invaild buf / CreatePlayerPosSyncPacket");
        //    return null;
        //}

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYERPOSSYNC, flatBufferSize = (ushort)data.Length };

        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] CreateHeartBeatPacket()
    {
        long currTime = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();

        var builder = new FlatBufferBuilder(1);
        HeartBeat.StartHeartBeat(builder);
        HeartBeat.AddTime(builder, currTime);
        var offset = HeartBeat.EndHeartBeat(builder);
        builder.Finish(offset.Value);

        //�������� ���� �̻���� �� �дµ� Verity�� false�� ��...
        var buf = builder.DataBuffer;
        var timeCheck = HeartBeat.GetRootAsHeartBeat(buf).Time;
        if (timeCheck != currTime)
        {
            Debug.Log("invaild buf / CreateHeartBeatPacket");

            return null;
        }

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_HEARTBEAT, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);

        byte[] result = new byte[data.Length + headerdata.Length];
        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);
        return result;
    }

    public void SendPlayerMovePacket(Vector3 position, Vector3 direction, int id, ePlayerState state)
    {

        byte[] packet = CreatePlayerMovePacket(position, direction, id, state);
        if (packet == null) { return; }
        SendPacket(packet);
    }

    public void SendPlayerStopPacket(Vector3 position, Vector3 direction, int id, ePlayerState state)
    {

        byte[] packet = CreatePlayerStopPacket(position, direction, id, state);
        if (packet == null) { return; }
        SendPacket(packet);
    }

    public void SendPlayerPosPacket(Vector3 position, Vector3 direction, int id)
    {
        byte[] packet = CreatePlayerPosSyncPacket(position, direction, id);
        if (packet == null) { return; }
        SendPacket(packet);
    }

    public void SendHeartBeatPacket()
    {
        byte[] packet = CreateHeartBeatPacket();
        if (packet == null) { return; }
        SendPacket(packet);
    }
}
