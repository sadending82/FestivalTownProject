using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using UnityEngine;
using NetworkProtocol;
using Google.FlatBuffers;
using Network.PacketProcessor;
using UnityEngine.UIElements;
using PacketTable.PlayerTable;
using PacketTable.UtilitiesTable;
using PacketTable.GameTable;
using PacketTable.ObjectTable;
using PacketTable.LobbyTable;
using System.Diagnostics;

public class PacketMaker
{

    public PacketMaker()
    {


    }
    public void Init()
    {

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

    public byte[] MakePlayerMovePacket(Vector3 position, Vector3 direction, int id, ePlayerMoveState state)
    {
        var builder = new FlatBufferBuilder(1);
        var pos = Vec3f.CreateVec3f(builder, position.x, position.y, position.z);
        var dir = Vec3f.CreateVec3f(builder, direction.x, direction.y, direction.z);
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

    public byte[] MakePlayerStopPacket(Vector3 position, Vector3 direction, int id, ePlayerMoveState state)
    {
        var builder = new FlatBufferBuilder(1);
        var pos = Vec3f.CreateVec3f(builder, position.x, position.y, position.z);
        var dir = Vec3f.CreateVec3f(builder, direction.x, direction.y, direction.z);
        PlayerStop.StartPlayerStop(builder);
        PlayerStop.AddPos(builder, pos);
        PlayerStop.AddDirection(builder, dir);
        PlayerStop.AddId(builder, id);
        PlayerStop.AddState(builder, (int)state);
        var offset = PlayerStop.EndPlayerStop(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYERSTOP, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakePlayerPosSyncPacket(Vector3 position, Vector3 direction, int id)
    {
        var builder = new FlatBufferBuilder(1);
        var pos = Vec3f.CreateVec3f(builder, position.x, position.y, position.z);
        var dir = Vec3f.CreateVec3f(builder, direction.x, direction.y, direction.z);
        PlayerPos.StartPlayerPos(builder);
        PlayerPos.AddPos(builder, pos);
        PlayerPos.AddDirection(builder, dir);
        PlayerPos.AddId(builder, id);
        var offset = PlayerPos.EndPlayerPos(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYERPOSSYNC, flatBufferSize = (ushort)data.Length };

        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakePlayerAnimationPacket(Vector3 position, Vector3 direction, int id, int animation)
    {
        var builder = new FlatBufferBuilder(1);
        var pos = Vec3f.CreateVec3f(builder, position.x, position.y, position.z);
        var dir = Vec3f.CreateVec3f(builder, direction.x, direction.y, direction.z);
        PlayerAnimation.StartPlayerAnimation(builder);
        PlayerAnimation.AddPos(builder, pos);
        PlayerAnimation.AddDirection(builder, dir);
        PlayerAnimation.AddId(builder, id);
        PlayerAnimation.AddAnimation(builder, animation);
        var offset = PlayerAnimation.EndPlayerAnimation(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYERANIMATION, flatBufferSize = (ushort)data.Length };

        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakePlayerDamageReceivePacket(int attacker_id, int target_id, int weapon, int attack_type, Vector3 knockback_direction)
    {
        var builder = new FlatBufferBuilder(1);
        var dir = Vec3f.CreateVec3f(builder, knockback_direction.x, knockback_direction.y, knockback_direction.z);
        PlayerDamageReceive.StartPlayerDamageReceive(builder);
        PlayerDamageReceive.AddAttackerId(builder, attacker_id);
        PlayerDamageReceive.AddTargetId(builder, target_id);
        PlayerDamageReceive.AddWeapon(builder, weapon);
        PlayerDamageReceive.AddAttackType(builder, attack_type);
        PlayerDamageReceive.AddKnockbackDirection(builder, dir);
        var offset = PlayerDamageReceive.EndPlayerDamageReceive(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYERDAMAGERECEIVE, flatBufferSize = (ushort)data.Length };

        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakeHeartBeatPacket()
    {
        long currTime = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();

        var builder = new FlatBufferBuilder(1);
        HeartBeat.StartHeartBeat(builder);
        HeartBeat.AddTime(builder, currTime);
        var offset = HeartBeat.EndHeartBeat(builder);
        builder.Finish(offset.Value);

        //서버에서 버퍼 이상없이 잘 읽는데 Verity가 false가 뜸...
        var buf = builder.DataBuffer;
        var timeCheck = HeartBeat.GetRootAsHeartBeat(buf).Time;
        if (timeCheck != currTime)
        {
            UnityEngine.Debug.Log("invaild buf / CreateHeartBeatPacket");

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

    public byte[] MakeBombInputPacket(int playerid, int bombid, int team)
    {
        var builder = new FlatBufferBuilder(1);
        BombInput.StartBombInput(builder);
        BombInput.AddId(builder, playerid);
        BombInput.AddTeam(builder, team);
        BombInput.AddBombid(builder, bombid);
        var offset = BombInput.EndBombInput(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_BOMBINPUT, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakePlayerGrabBombPacket(Vector3 position, Vector3 direction, int playerID, int BombID)
    {
        var builder = new FlatBufferBuilder(1);
        var pos = Vec3f.CreateVec3f(builder, position.x, position.y, position.z);
        var dir = Vec3f.CreateVec3f(builder, direction.x, direction.y, direction.z);
        PlayerGrabBomb.StartPlayerGrabBomb(builder);
        PlayerGrabBomb.AddId(builder, playerID);
        PlayerGrabBomb.AddPos(builder, pos);
        PlayerGrabBomb.AddDirection(builder, dir);
        PlayerGrabBomb.AddBombId(builder, BombID);
        var offset = PlayerGrabBomb.EndPlayerGrabBomb(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYERGRABBOMB, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakePlayerThrowBombPacket(Vector3 position, Vector3 direction, int playerID, int BombID)
    {
        var builder = new FlatBufferBuilder(1);
        var pos = Vec3f.CreateVec3f(builder, position.x, position.y, position.z);
        var dir = Vec3f.CreateVec3f(builder, direction.x, direction.y, direction.z);
        PlayerThrowBomb.StartPlayerThrowBomb(builder);
        PlayerThrowBomb.AddId(builder, playerID);
        PlayerThrowBomb.AddPos(builder, pos);
        PlayerThrowBomb.AddDirection(builder, dir);
        PlayerThrowBomb.AddBombId(builder, BombID);
        var offset = PlayerThrowBomb.EndPlayerThrowBomb(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYERTHROWBOMB, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakeBombPositionSyncPacket(Vector3 position, int BombID)
    {
        var builder = new FlatBufferBuilder(1);
        var pos = Vec3f.CreateVec3f(builder, position.x, position.y, position.z);
        BombPosition.StartBombPosition(builder);
        BombPosition.AddId(builder, BombID);
        BombPosition.AddPos(builder, pos);
        var offset = BombPosition.EndBombPosition(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_BOMBPOSSYNC, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakeBombExplosionPacket(Vector3 position, int BombID)
    {
        var builder = new FlatBufferBuilder(1);
        var pos = Vec3f.CreateVec3f(builder, position.x, position.y, position.z);
        BombExplosion.StartBombExplosion(builder);
        BombExplosion.AddId(builder, BombID);
        BombExplosion.AddPos(builder, pos);
        var offset = BombExplosion.EndBombExplosion(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_BOMBEXPLOSION, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakeGameMatchingRequestPacket()
    {
        var builder = new FlatBufferBuilder(1);
        GameMatchingRequest.StartGameMatchingRequest(builder);
        var offset = GameMatchingRequest.EndGameMatchingRequest(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_MATCHINGREQUEST, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakeGameReadyPacket()
    {
        var builder = new FlatBufferBuilder(1);
        GameReady.StartGameReady(builder);
        var offset = GameReady.EndGameReady(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_GAMEREADY, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }
}
