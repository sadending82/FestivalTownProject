﻿using System;
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
using PacketTable.LoginTable;
using PacketTable.CheatTable;
using PacketTable.PassTable;
using PacketTable.ShopTable;

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

    public byte[] MakeHeartBeatPacket()
    {
        long currTime = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();

        var builder = new FlatBufferBuilder(1);
        HeartBeat.StartHeartBeat(builder);
        HeartBeat.AddTime(builder, currTime);
        var offset = HeartBeat.EndHeartBeat(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_HEART_BEAT, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);

        byte[] result = new byte[data.Length + headerdata.Length];
        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);
        return result;
    }

    public byte[] MakePingCheckPacket()
    {
        var builder = new FlatBufferBuilder(1);
        PingCheck.StartPingCheck(builder);
        var offset = PingCheck.EndPingCheck(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.PING_CHECK, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);

        byte[] result = new byte[data.Length + headerdata.Length];
        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);
        return result;
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

    public byte[] MakePlayerDropBombPacket(Vector3 position, int playerID, int BombID)
    {
        var builder = new FlatBufferBuilder(1);
        var pos = Vec3f.CreateVec3f(builder, position.x, position.y, position.z);
        PlayerDropBomb.StartPlayerDropBomb(builder);
        PlayerDropBomb.AddId(builder, playerID);
        PlayerDropBomb.AddPos(builder, pos);
        PlayerDropBomb.AddBombId(builder, BombID);
        var offset = PlayerDropBomb.EndPlayerDropBomb(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PLAYER_DROP_BOMB, flatBufferSize = (ushort)data.Length };
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

    public byte[] MakePlayerGrabOtherPlayerPacket(int playerID, Vector3 myPosition, Vector3 myDirection, Vector3 myHandPosition, bool isLeftHand, int targetID, Vector3 targetHeadPosition)
    {
        var builder = new FlatBufferBuilder(1);

        var my_pos = Vec3f.CreateVec3f(builder, myPosition.x, myPosition.y, myPosition.z);
        var my_dir = Vec3f.CreateVec3f(builder, myDirection.x, myDirection.y, myDirection.z);
        var my_hand_pos = Vec3f.CreateVec3f(builder, myHandPosition.x, myHandPosition.y, myHandPosition.z);

        var target_head_pos = Vec3f.CreateVec3f(builder, targetHeadPosition.x, targetHeadPosition.y, targetHeadPosition.z);

        PlayerGrabOtherPlayer.StartPlayerGrabOtherPlayer(builder);
        PlayerGrabOtherPlayer.AddId(builder, playerID);
        PlayerGrabOtherPlayer.AddPos(builder, my_pos);
        PlayerGrabOtherPlayer.AddDirection(builder, my_dir);
        PlayerGrabOtherPlayer.AddHandPos(builder, my_hand_pos);
        PlayerGrabOtherPlayer.AddIsLeftHand(builder, isLeftHand);

        PlayerGrabOtherPlayer.AddTargetId(builder, targetID);
        PlayerGrabOtherPlayer.AddTargetHeadPos(builder, target_head_pos);

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

    public byte[] MakePlayerThrowOtherPlayerPacket(int playerID, Vector3 myPosition, Vector3 myDirection, int targetID, Vector3 targetPosition, Vector3 targetDirection)
    {
        var builder = new FlatBufferBuilder(1);

        var my_pos = Vec3f.CreateVec3f(builder, myPosition.x, myPosition.y, myPosition.z);
        var my_dir = Vec3f.CreateVec3f(builder, myDirection.x, myDirection.y, myDirection.z);
        var target_pos = Vec3f.CreateVec3f(builder, targetPosition.x, targetPosition.y, targetPosition.z);
        var target_dir = Vec3f.CreateVec3f(builder, targetDirection.x, targetDirection.y, targetDirection.z);

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

    public byte[] MakeGameMatchingRequestPacket(eMatchingType type, int map_code = 0)
    {
        var builder = new FlatBufferBuilder(1);
        GameMatchingRequest.StartGameMatchingRequest(builder);
        GameMatchingRequest.AddMatchingType(builder, (int)type);
        GameMatchingRequest.AddMapCode(builder, map_code);
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

    public byte[] MakeGachaRequestPacket(int gachaType, int pay_item)
    {
        var builder = new FlatBufferBuilder(1);
        GachaRequest.StartGachaRequest(builder);
        GachaRequest.AddRandomBoxIndex(builder, gachaType);
        GachaRequest.AddPayItem(builder, pay_item);
        var offset = GachaRequest.EndGachaRequest(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_GACHA_REQUEST, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakeCurrencyAmountRequestPacket()
    {
        var builder = new FlatBufferBuilder(1);
        CurrencyAmountRequest.StartCurrencyAmountRequest(builder);
        var offset = CurrencyAmountRequest.EndCurrencyAmountRequest(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_CURRENCY_AMOUNT_REQUEST, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakeUserItemsRequestPacket()
    {
        var builder = new FlatBufferBuilder(1);
        UserItemsRequest.StartUserItemsRequest(builder);
        var offset = UserItemsRequest.EndUserItemsRequest(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_USER_ITEMS_REQUEST, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }
    public byte[] MakeChangeCharacterCustomizingPacket(sCharacterCustomizing customizing)
    {
        var builder = new FlatBufferBuilder(1);
        var itemList = customizing.itemList;
        var items = new Offset<CustomizingItem>[customizing.itemList.Count];

        int count = 0;
        foreach (var item in itemList)
        {
            items[count] = CustomizingItem.CreateCustomizingItem(builder, item.Value.itemType, item.Value.item_UID, item.Value.itemCode);
            count++;
        }

        var itemVector = ChangeCharacterCustomizing.CreateCustomizingItemsVector(builder, items);

        ChangeCharacterCustomizing.StartChangeCharacterCustomizing(builder);
        ChangeCharacterCustomizing.AddCustomizingItems(builder, itemVector);
        var offset = ChangeCharacterCustomizing.EndChangeCharacterCustomizing(builder);

        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_CHANGE_CHARACTER_CUSTOMIZING, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakeAttendanceEventRequestPacket(int eventCode)
    {
        var builder = new FlatBufferBuilder(1);

        AttendanceEventRequest.StartAttendanceEventRequest(builder);
        AttendanceEventRequest.AddEventCode(builder, eventCode);
        var offset = AttendanceEventRequest.EndAttendanceEventRequest(builder);

        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_ATTENDANCE_EVENT_REQUEST, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }
    public byte[] MakeAttendanceRewardRequestPacket(int eventCode, int dayCount)
    {
        var builder = new FlatBufferBuilder(1);

        AttendanceRewardRequest.StartAttendanceRewardRequest(builder);
        AttendanceRewardRequest.AddEventCode(builder, eventCode);
        AttendanceRewardRequest.AddDayCount(builder, dayCount); 
        var offset = AttendanceRewardRequest.EndAttendanceRewardRequest(builder);

        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_ATTENDANCE_REWARD_REQUEST, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }
    public byte[] MakeEventRewardRequestPacket(int eventCode)
    {
        //var builder = new FlatBufferBuilder(1);

        //EventRewardRequest.StartEventRewardRequest(builder);
        //EventRewardRequest.AddEventCode(builder, eventCode);
        //var offset = EventRewardRequest.EndEventRewardRequest(builder);

        //builder.Finish(offset.Value);

        //byte[] data = builder.SizedByteArray();
        //HEADER header = new HEADER { type = (ushort)ePacketType.C2S_EVENT_REWARD_REQUEST, flatBufferSize = (ushort)data.Length };
        //byte[] headerdata = Serialize<HEADER>(header);
        //byte[] result = new byte[data.Length + headerdata.Length];

        //Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        //Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        //return result;
        return null;
    }

    public byte[] MakeUserPassStateRequestPacket()
    {
        var builder = new FlatBufferBuilder(1);

        UserPassStateRequest.StartUserPassStateRequest(builder);
        var offset = UserPassStateRequest.EndUserPassStateRequest(builder);

        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PASS_STATE_REQUEST, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakeUserMissionStateRequestPacket()
    {
        var builder = new FlatBufferBuilder(1);

        UserMissionStateRequest.StartUserMissionStateRequest(builder);
        var offset = UserMissionStateRequest.EndUserMissionStateRequest(builder);

        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PASS_MISSION_STATE_REQUEST, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakeMissionCompleteRequestPacket(int mission_index)
    {
        var builder = new FlatBufferBuilder(1);

        MissionCompleteRequest.StartMissionCompleteRequest(builder);
        MissionCompleteRequest.AddMissionIndex(builder, mission_index);
        var offset = MissionCompleteRequest.EndMissionCompleteRequest(builder);

        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_MISSION_COMPLETE_REQUEST, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }
    public byte[] MakePassRewardRequestPacket(int pass_index, int pass_type, int pass_level)
    {
        var builder = new FlatBufferBuilder(1);

        PassRewardRequest.StartPassRewardRequest(builder);
        PassRewardRequest.AddPassIndex(builder, pass_index);
        PassRewardRequest.AddPassType(builder, pass_type);
        PassRewardRequest.AddPassLevel(builder, pass_level);    
        var offset = PassRewardRequest.EndPassRewardRequest(builder);

        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PASS_REWARD_REQUEST, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }
    public byte[] MakeBatchReceivePassRewardsRequestPacket(int pass_index)
    {
        var builder = new FlatBufferBuilder(1);

        BatchReceivePassRewardsRequest.StartBatchReceivePassRewardsRequest(builder);
        BatchReceivePassRewardsRequest.AddPassIndex(builder, pass_index);
        var offset = BatchReceivePassRewardsRequest.EndBatchReceivePassRewardsRequest(builder);

        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_BATCH_RECEIVE_PASS_REWARDS_REQUEST, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }
    public byte[] MakePurchaseGoodsRequestPacket(int goods_index)
    {
        var builder = new FlatBufferBuilder(1);

        PurchaseGoodsRequest.StartPurchaseGoodsRequest(builder);
        PurchaseGoodsRequest.AddIndex(builder, goods_index);
        var offset = PurchaseGoodsRequest.EndPurchaseGoodsRequest(builder);

        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_PURCHASE_GOODS_REQUEST, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    // player
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

    public byte[] MakeVersionCheckRequestPacket(string version)
    {
        var builder = new FlatBufferBuilder(1);
        var Version = builder.CreateString(version);

        VersionCheckRequest.StartVersionCheckRequest(builder);
        VersionCheckRequest.AddVersion(builder, Version);
        var offset = VersionCheckRequest.EndVersionCheckRequest(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_VERSION_CHECK_REQUEST, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakeLoginRequestPacket(string accountID, string accountPassword)
    {
        var builder = new FlatBufferBuilder(1);
        var AccountID = builder.CreateString(accountID);
        var AccountPassword = builder.CreateString(accountPassword);
        LoginRequest.StartLoginRequest(builder);
        LoginRequest.AddAccountId(builder, AccountID);
        LoginRequest.AddAccountPassword(builder, AccountPassword);
        var offset = LoginRequest.EndLoginRequest(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_LOGIN_REQUEST, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakeSignUpRequestPacket(string accountID, string accountPassword, string nickName)
    {
        var builder = new FlatBufferBuilder(1);
        var AccountID = builder.CreateString(accountID);
        var AccountPassword = builder.CreateString(accountPassword);
        var NickName = builder.CreateString(nickName);
        SignUpRequest.StartSignUpRequest(builder);
        SignUpRequest.AddAccountId(builder, AccountID);
        SignUpRequest.AddAccountPassword(builder, AccountPassword);
        SignUpRequest.AddNickname(builder,NickName);
        var offset = SignUpRequest.EndSignUpRequest(builder);
        builder.Finish(offset.Value);
        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_SIGNUP_REQUEST, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];
        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);
        return result;
    }

    public byte[] MakeCheckID_DuplicationPacket(string accountID)
    {
        var builder = new FlatBufferBuilder(1);
        var AccountID = builder.CreateString(accountID);
        CheckID_Duplication.StartCheckID_Duplication(builder);
        CheckID_Duplication.AddId(builder, AccountID);
 
        var offset = CheckID_Duplication.EndCheckID_Duplication(builder);
        builder.Finish(offset.Value);
        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_CHECK_ID_DUPLICATION, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];
        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);
        return result;
    }


    // ------------------ Cheat ------------------
    public byte[] MakeAccountResetPacket()
    {
        var builder = new FlatBufferBuilder(1);
        AccountReset.StartAccountReset(builder);
        var offset = AccountReset.EndAccountReset(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_ACCOUNT_RESET, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }

    public byte[] MakeGetGoldCheatPacket()
    {
        var builder = new FlatBufferBuilder(1);
        GetCurrency.StartGetCurrency(builder);
        GetCurrency.AddItemCode(builder, 100001);
        GetCurrency.AddCount(builder, 10000);
        var offset = GetCurrency.EndGetCurrency(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_GET_CURRENCY, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }
    public byte[] MakeGetDiaCheatPacket()
    {
        var builder = new FlatBufferBuilder(1);
        GetCurrency.StartGetCurrency(builder);
        GetCurrency.AddItemCode(builder, 100002);
        GetCurrency.AddCount(builder, 10000);
        var offset = GetCurrency.EndGetCurrency(builder);
        builder.Finish(offset.Value);

        byte[] data = builder.SizedByteArray();
        HEADER header = new HEADER { type = (ushort)ePacketType.C2S_GET_CURRENCY, flatBufferSize = (ushort)data.Length };
        byte[] headerdata = Serialize<HEADER>(header);
        byte[] result = new byte[data.Length + headerdata.Length];

        Buffer.BlockCopy(headerdata, 0, result, 0, headerdata.Length);
        Buffer.BlockCopy(data, 0, result, headerdata.Length, data.Length);

        return result;
    }
}
