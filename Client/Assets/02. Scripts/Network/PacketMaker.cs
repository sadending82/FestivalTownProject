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
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYER_MOVE, flatBufferSize = (ushort)data.Length };

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
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYER_STOP, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakePlayerSyncPacket(Vector3 position, Vector3 direction, int stamina, int id)
    {
        var builder = new FlatBufferBuilder(1);
        var pos = Vec3f.CreateVec3f(builder, position.x, position.y, position.z);
        var dir = Vec3f.CreateVec3f(builder, direction.x, direction.y, direction.z);
        PlayerSync.StartPlayerSync(builder);
        PlayerSync.AddPos(builder, pos);
        PlayerSync.AddDirection(builder, dir);
        PlayerSync.AddStamina(builder, stamina);
        PlayerSync.AddId(builder, id);
        var offset = PlayerSync.EndPlayerSync(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYER_SYNC, flatBufferSize = (ushort)data.Length };

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
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYER_ANIMATION, flatBufferSize = (ushort)data.Length };

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
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYER_DAMAGE_RECEIVE, flatBufferSize = (ushort)data.Length };

        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakePlayerCollisionToBlockPacket(int id)
    {
        var builder = new FlatBufferBuilder(1);
        PlayerCollisionToBlock.StartPlayerCollisionToBlock(builder);
        PlayerCollisionToBlock.AddId(builder, id);
        var offset = PlayerCollisionToBlock.EndPlayerCollisionToBlock(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYER_COLLISION_BLOCK, flatBufferSize = (ushort)data.Length };

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
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_HEART_BEAT, flatBufferSize = (ushort)data.Length };
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
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_BOMB_INPUT, flatBufferSize = (ushort)data.Length };
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
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYER_GRAB_BOMB, flatBufferSize = (ushort)data.Length };
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
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYER_THROW_BOMB, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakePlayerGrabOtherPlayerPacket(int playerID, Vector3 my_position, Vector3 my_direction, int targetID, Vector3 target_position)
    {
        var builder = new FlatBufferBuilder(1);

        var my_pos = Vec3f.CreateVec3f(builder, my_position.x, my_position.y, my_position.z);
        var my_dir = Vec3f.CreateVec3f(builder, my_direction.x, my_direction.y, my_direction.z);
        var target_pos = Vec3f.CreateVec3f(builder, target_position.x, target_position.y, target_position.z);
        var target_dir = Vec3f.CreateVec3f(builder, 0, 0, 0);

        PlayerGrabOtherPlayer.StartPlayerGrabOtherPlayer(builder);
        PlayerGrabOtherPlayer.AddId(builder, playerID);
        PlayerGrabOtherPlayer.AddPos(builder, my_pos);
        PlayerGrabOtherPlayer.AddDirection(builder, my_dir);

        PlayerGrabOtherPlayer.AddTargetId(builder, targetID);
        PlayerGrabOtherPlayer.AddTargetPos(builder, target_pos);
        PlayerGrabOtherPlayer.AddTargetDirection(builder, target_dir);

        var offset = PlayerGrabOtherPlayer.EndPlayerGrabOtherPlayer(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYER_GRAB_OTHER_PLAYER, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakePlayerGrabOtherPlayerPacket(int playerID, Vector3 my_position, Vector3 my_direction, int targetID, Vector3 target_position, Vector3 target_direction)
    {
        var builder = new FlatBufferBuilder(1);

        var my_pos = Vec3f.CreateVec3f(builder, my_position.x, my_position.y, my_position.z);
        var my_dir = Vec3f.CreateVec3f(builder, my_direction.x, my_direction.y, my_direction.z);
        var target_pos = Vec3f.CreateVec3f(builder, target_position.x, target_position.y, target_position.z);
        var target_dir = Vec3f.CreateVec3f(builder, target_direction.x, target_direction.y, target_direction.z);

        PlayerGrabOtherPlayer.StartPlayerGrabOtherPlayer(builder);
        PlayerGrabOtherPlayer.AddId(builder, playerID);
        PlayerGrabOtherPlayer.AddPos(builder, my_pos);
        PlayerGrabOtherPlayer.AddDirection(builder, my_dir);

        PlayerGrabOtherPlayer.AddTargetId(builder, targetID);
        PlayerGrabOtherPlayer.AddTargetPos(builder, target_pos);
        PlayerGrabOtherPlayer.AddTargetDirection(builder, target_dir);

        var offset = PlayerGrabOtherPlayer.EndPlayerGrabOtherPlayer(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYER_GRAB_OTHER_PLAYER, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakePlayerThrowOtherPlayerPacket(int playerID, Vector3 my_position, Vector3 my_direction, int targetID, Vector3 target_position)
    {
        var builder = new FlatBufferBuilder(1);

        var my_pos = Vec3f.CreateVec3f(builder, my_position.x, my_position.y, my_position.z);
        var my_dir = Vec3f.CreateVec3f(builder, my_direction.x, my_direction.y, my_direction.z);
        var target_pos = Vec3f.CreateVec3f(builder, target_position.x, target_position.y, target_position.z);
        var target_dir = Vec3f.CreateVec3f(builder, 0, 0, 0);

        PlayerThrowOtherPlayer.StartPlayerThrowOtherPlayer(builder);
        PlayerThrowOtherPlayer.AddId(builder, playerID);
        PlayerThrowOtherPlayer.AddPos(builder, my_pos);
        PlayerThrowOtherPlayer.AddDirection(builder, my_dir);

        PlayerThrowOtherPlayer.AddTargetId(builder, targetID);
        PlayerThrowOtherPlayer.AddTargetPos(builder, target_pos);
        PlayerThrowOtherPlayer.AddTargetDirection(builder, target_dir);

        var offset = PlayerThrowOtherPlayer.EndPlayerThrowOtherPlayer(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYER_THROW_OTHER_PLAYER, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakePlayerThrowOtherPlayerPacket(int playerID, Vector3 my_position, Vector3 my_direction, int targetID, Vector3 target_position, Vector3 target_direction)
    {
        var builder = new FlatBufferBuilder(1);

        var my_pos = Vec3f.CreateVec3f(builder, my_position.x, my_position.y, my_position.z);
        var my_dir = Vec3f.CreateVec3f(builder, my_direction.x, my_direction.y, my_direction.z);
        var target_pos = Vec3f.CreateVec3f(builder, target_position.x, target_position.y, target_position.z);
        var target_dir = Vec3f.CreateVec3f(builder, target_direction.x, target_direction.y, target_direction.z);

        PlayerThrowOtherPlayer.StartPlayerThrowOtherPlayer(builder);
        PlayerThrowOtherPlayer.AddId(builder, playerID);
        PlayerThrowOtherPlayer.AddPos(builder, my_pos);
        PlayerThrowOtherPlayer.AddDirection(builder, my_dir);

        PlayerThrowOtherPlayer.AddTargetId(builder, targetID);
        PlayerThrowOtherPlayer.AddTargetPos(builder, target_pos);
        PlayerThrowOtherPlayer.AddTargetDirection(builder, target_dir);

        var offset = PlayerThrowOtherPlayer.EndPlayerThrowOtherPlayer(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYER_THROW_OTHER_PLAYER, flatBufferSize = (ushort)data.Length };
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
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_BOMB_POS_SYNC, flatBufferSize = (ushort)data.Length };
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
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_BOMB_EXPLOSION, flatBufferSize = (ushort)data.Length };
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
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_MATCHING_REQUEST, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakeGameMatchingCancelPacket()
    {
        var builder = new FlatBufferBuilder(1);
        GameMatchingCancel.StartGameMatchingCancel(builder);
        var offset = GameMatchingRequest.EndGameMatchingRequest(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_MATCHING_CANCEL, flatBufferSize = (ushort)data.Length };
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
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_GAME_READY, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakePlayerGrabWeaponPacket(Vector3 position, Vector3 direction, int playerID, int weaponID)
    {
        var builder = new FlatBufferBuilder(1);
        var pos = Vec3f.CreateVec3f(builder, position.x, position.y, position.z);
        var dir = Vec3f.CreateVec3f(builder, direction.x, direction.y, direction.z);
        PlayerGrabWeapon.StartPlayerGrabWeapon(builder);
        PlayerGrabWeapon.AddId(builder, playerID);
        PlayerGrabWeapon.AddPos(builder, pos);
        PlayerGrabWeapon.AddDirection(builder, dir);
        PlayerGrabWeapon.AddWeaponId(builder, weaponID);
        var offset = PlayerGrabWeapon.EndPlayerGrabWeapon(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYER_GRAB_WEAPON, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakePlayerDropWeaponPacket(Vector3 position, int weaponID)
    {
        var builder = new FlatBufferBuilder(1);
        var pos = Vec3f.CreateVec3f(builder, position.x, position.y, position.z);
        WeaponPosition.StartWeaponPosition(builder);
        WeaponPosition.AddPos(builder, pos);
        WeaponPosition.AddId(builder, weaponID);
        var offset = WeaponPosition.EndWeaponPosition(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYER_DROP_WEAPON, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakeWeaponDeletePacket(int weaponID)
    {
        var builder = new FlatBufferBuilder(1);
        WeaponDelete.StartWeaponDelete(builder);
        WeaponDelete.AddId(builder, weaponID);
        var offset = WeaponDelete.EndWeaponDelete(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_WEAPON_DELETE, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }
}
