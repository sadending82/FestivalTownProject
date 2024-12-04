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
using PacketTable.PlayerTable;
using static UnityEngine.GraphicsBuffer;
using TMPro;

public class PacketManager : MonoBehaviour 
{
    private bool isGameEnd = false;

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
            { ePacketType.S2C_VERSION_CHECK_RESPONSE, new VersionCheckResponseProcessor() },
            { ePacketType.S2C_LOGIN_RESPONSE, new LoginResponseProcessor() },
            { ePacketType.S2C_SIGNUP_RESPONSE, new SignUpResponseProcessor() },

            { ePacketType.S2C_GACHA_RESPONSE, new GachaResponseProcessor() },
            { ePacketType.S2C_CURRENCY_AMOUNT_RESPONSE, new CurrencyAmountResponseProcessor() },
            { ePacketType.S2C_USER_ITEMS_RESPONSE, new UserItemsResponseProcessor() },
            { ePacketType.S2C_CHANGE_CHARACTER_CUSTOMIZING, new ChangeCharacterCustomizingProcessor() },
            { ePacketType.S2C_ATTENDANCE_EVENT_RESPONSE, new AttendanceEventResponseProcessor() },
            { ePacketType.S2C_ATTENDANCE_REWARD_RESPONSE, new AttendanceRewardResponseProcessor() },

            { ePacketType.S2C_HEART_BEAT, new HeartBeatProcessor() },

            { ePacketType.S2C_PLAYER_ADD, new PlayerAddProcessor() },
            { ePacketType.S2C_PLAYER_DELETE, new PlayerDeleteProcessor() },
            { ePacketType.S2C_PLAYER_MOVE, new PlayerMoveProcessor() },
            { ePacketType.S2C_PLAYER_STOP, new PlayerStopProcessor() },
            { ePacketType.S2C_PLAYER_SYNC, new PlayerSyncProcessor() },
            { ePacketType.S2C_PLAYER_GRAB_BOMB, new PlayerGrabBombProcessor() },
            { ePacketType.S2C_PLAYER_DROP_BOMB, new PlayerDropBombProcessor() },
            { ePacketType.S2C_PLAYER_THROW_BOMB, new PlayerThrowBombProcessor() },
            { ePacketType.S2C_PLAYER_GRAB_WEAPON, new PlayerGrabWeaponProcessor() },
            { ePacketType.S2C_PLAYER_DROP_WEAPON, new PlayerDropWeaponProcessor() },
            { ePacketType.S2C_PLAYER_ANIMATION, new PlayerAnimationProcessor() },
            { ePacketType.S2C_PLAYER_CALCULATED_DAMAGE, new PlayerCalculatedDamageProcessor() },
            { ePacketType.S2C_PLAYER_DEAD, new PlayerDeadProcessor() },
            { ePacketType.S2C_PLAYER_RESPAWN, new PlayerRespawnProcessor() },
            { ePacketType.S2C_PLAYER_GROGGY, new PlayerGroggyProcessor() },
            { ePacketType.S2C_PLAYER_GROGGY_RECOVERY, new PlayerGroggyRecoveryProcessor() },
            { ePacketType.S2C_PLAYER_GRAB_OTHER_PLAYER, new PlayerGrabOtherPlayerProcessor() },
            { ePacketType.S2C_PLAYER_THROW_OTHER_PLAYER, new PlayerThrowOtherPlayerProcessor() },

            { ePacketType.S2C_BLOCK_DROP, new BlockDropProcessor() },
            { ePacketType.S2C_BOMB_SPAWN, new BombSpawnProcessor() },
            { ePacketType.S2C_BOMB_EXPLOSION, new BombExplosionProcessor() },
            { ePacketType.S2C_BOMB_POS_SYNC, new BombPositionSyncProcessor() },
            { ePacketType.S2C_WEAPON_SPAWN, new WeaponSpawnProcessor() },
            { ePacketType.S2C_WEAPON_DELETE, new WeaponDeleteProcessor() },

            { ePacketType.S2C_LIFE_REDUCE, new LifeReduceProcessor() },
            { ePacketType.S2C_REMAIN_TIME_SYNC, new RemainTimeSyncProcessor() },

            { ePacketType.S2C_MATCHING_RESPONSE, new GameMatchingResponseProcessor() },
            { ePacketType.S2C_MATCHING_CANCEL, new GameMatchingCancelProcessor() },
            { ePacketType.S2C_ALL_PLAYER_READY, new AllPlayerReadyProcessor() },
            { ePacketType.S2C_GAME_START, new GameStartProcessor() },
            { ePacketType.S2C_GAME_END, new GameEndProcessor() },
            { ePacketType.S2C_GAME_RESULT, new GameResultProcessor() },
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
    private void SendPacketWhenPlayingGame(Byte[] buffer)
    {
        if (isGameEnd) return;
        SendPacket(buffer);
    }
    public void GameStart()
    {
        isGameEnd = false;
    }
    public void GameEnd()
    {
        isGameEnd = true;
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

    // ------------------ Game ------------------
    public void SendHeartBeatPacket()
    {
        byte[] packet = _packetMaker.MakeHeartBeatPacket();
        if (packet == null) { return; }
        SendPacket(packet);
    }
    public void SendVersionCheckRequestPacket()
    {
        byte[] packet = _packetMaker.MakeVersionCheckRequestPacket(Application.version);
        if (packet == null) { return; }
        SendPacket(packet);
    }
    public void SendLoginRequestPacket(string accountID, string accountPassword)
    {
        byte[] packet = _packetMaker.MakeLoginRequestPacket(accountID, accountPassword);
        if (packet == null) { return; }
        SendPacket(packet);
    }

    public void SendSignUpRequestPacket(string accountID, string accountPassword, string nickname)
    {
        byte[] packet = _packetMaker.MakeSignUpRequestPacket(accountID, accountPassword, nickname);
        if (packet == null) { return; }
        SendPacket(packet);
    }
    public void SendGameMatchingRequest(eMatchingType type)
    {
        byte[] packet = _packetMaker.MakeGameMatchingRequestPacket(type);
        if (packet == null) { return; }
        SendPacket(packet);
    }
    public void SendGameMatchingCancel()
    {
        byte[] packet = _packetMaker.MakeGameMatchingCancelPacket();
        if (packet == null) { return; }
        SendPacket(packet);
    }
    public void SendGameReady()
    {
        byte[] packet = _packetMaker.MakeGameReadyPacket();
        if (packet == null) { return; }
        SendPacket(packet);
    }
    public void SendGachaRequestPacket(int gachaType, int pay_item = 0)
    {
        byte[] packet = _packetMaker.MakeGachaRequestPacket(gachaType, pay_item);
        if (packet == null) { return; }
        SendPacket(packet);
    }
    public void SendCurrencyAmountRequestPacket()
    {
        byte[] packet = _packetMaker.MakeCurrencyAmountRequestPacket();
        if (packet == null) { return; }
        SendPacket(packet);
    }
    public void SendUserItemsRequestPacket()
    {
        byte[] packet = _packetMaker.MakeUserItemsRequestPacket();
        if (packet == null) { return; }
        SendPacket(packet);
    }
    public void SendChangeCharacterCustomizingPacket(sCharacterCustomizing customizing)
    {
        byte[] packet = _packetMaker.MakeChangeCharacterCustomizingPacket(customizing);
        if (packet == null) { return; }
        SendPacket(packet);
    }

    public void SendAttendanceEventRequestPacket(int eventCode)
    {
        byte[] packet = _packetMaker.MakeAttendanceEventRequestPacket(eventCode);
        if (packet == null) { return; }
        SendPacket(packet);
    }
    public void SendAttendanceRewardRequestPacket(int eventCode, int dayCount)
    {
        byte[] packet = _packetMaker.MakeAttendanceRewardRequestPacket(eventCode, dayCount);
        if (packet == null) { return; }
        SendPacket(packet);
    }

    // ------------------ Player ------------------
    public void SendPlayerSyncPacket(Vector3 position, Vector3 direction, int stamina, int id)
    {
        byte[] packet = _packetMaker.MakePlayerSyncPacket(position, direction, stamina, id);
        if (packet == null) { return; }
        SendPacket(packet);
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
    public void SendPlayerAnimationPacket(Vector3 position, Vector3 direction, int playerID, UpperBodyAnimationState animation)
    {
        byte[] packet = _packetMaker.MakePlayerAnimationPacket(position, direction, playerID, (int)animation);
        if (packet == null) { return; }
        SendPacket(packet);
    }
    public void SendPlayerGrabBombPacket(Vector3 position, Vector3 direction, int playerID, int BombID)
    {
        byte[] packet = _packetMaker.MakePlayerGrabBombPacket(position, direction, playerID, BombID);
        if (packet == null) { return; }
        SendPacket(packet);
    }
    public void SendPlayerDropBombPacket(Vector3 position, int playerID, int BombID)
    {
        byte[] packet = _packetMaker.MakePlayerDropBombPacket(position, playerID, BombID);
        if (packet == null) { return; }
        SendPacket(packet);
    }
    public void SendPlayerThrowBombPacket(Vector3 position, Vector3 direction, int playerID, int BombID)
    {
        byte[] packet = _packetMaker.MakePlayerThrowBombPacket(position, direction, playerID, BombID);
        if (packet == null) { return; }
        SendPacket(packet);
    }
    public void SendPlayerGrabWeaponPacket(Vector3 position, Vector3 direction, int playerID, int weaponID)
    {
        byte[] packet = _packetMaker.MakePlayerGrabWeaponPacket(position, direction, playerID, weaponID);
        if (packet == null) { return; }
        SendPacket(packet);
    }
    public void SendPlayerGrabOtherPlayerPacket(int playerID, Vector3 myPosition, Vector3 myDirection, Vector3 myHandPosition, bool isLeftHand, int targetID, Vector3 targetHeadPosition)
    {
        byte[] packet = _packetMaker.MakePlayerGrabOtherPlayerPacket(playerID, myPosition, myDirection, myHandPosition, isLeftHand, targetID, targetHeadPosition);
        if (packet == null) { return; }
        SendPacket(packet);
    }

    public void SendPlayerThrowOtherPlayerPacket(int playerID, Vector3 myPosition, Vector3 myDirection, int targetID, Vector3 targetPosition, Vector3 targetDirection)
    {
        byte[] packet = _packetMaker.MakePlayerThrowOtherPlayerPacket(playerID, myPosition, myDirection, targetID, targetPosition, targetDirection);
        if (packet == null) { return; }
        SendPacket(packet);
    }
    public void SendPlayerDropWeaponPacket(Vector3 position, int weaponID)
    {
        byte[] packet = _packetMaker.MakePlayerDropWeaponPacket(position, weaponID);
        if (packet == null) { return; }
        SendPacket(packet);
    }
    public void SendPlayerCollisionToBlockPacket(int id)
    {
        byte[] packet = _packetMaker.MakePlayerCollisionToBlockPacket(id);
        if (packet == null) { return; }
        SendPacketWhenPlayingGame(packet);
    }
    public void SendPlayerDamageReceivePacket(int attackerID, int targetID, int weapon, eDamageType attackType, Vector3 knockback_direction)
    {
        byte[] packet = _packetMaker.MakePlayerDamageReceivePacket(attackerID, targetID, weapon, (int)attackType, knockback_direction);
        if (packet == null) { return; }
        SendPacketWhenPlayingGame(packet);
    }

    // ------------------ Bomb & Weapon ------------------
    public void SendBombPositionSyncPacket(Vector3 position, int BombID)
    {
        byte[] packet = _packetMaker.MakeBombPositionSyncPacket(position, BombID);
        if (packet == null) { return; }
        SendPacketWhenPlayingGame(packet);
    }
    public void SendBombInputPacket(int bombid, int team, int playerid = 0)
    {
        byte[] packet = _packetMaker.MakeBombInputPacket(playerid, bombid, team);
        if (packet == null) { return; }
        SendPacketWhenPlayingGame(packet);
    }
    public void SendBombExplosionPacket(Vector3 position, int BombID)
    {
        byte[] packet = _packetMaker.MakeBombExplosionPacket(position, BombID);
        if (packet == null) { return; }
        SendPacketWhenPlayingGame(packet);
    }
    public void SendWeaponDeletePacket(int weaponID)
    {
        byte[] packet = _packetMaker.MakeWeaponDeletePacket(weaponID);
        if (packet == null) { return; }
        SendPacketWhenPlayingGame(packet);
    }

    // ------------------ Cheat ------------------
    public void SendAccountResetPacket()
    {
        byte[] packet = _packetMaker.MakeAccountResetPacket();
        if (packet == null) { return; }
        SendPacketWhenPlayingGame(packet);
    }
    public void SendGetGoldCheatPacket()
    {
        byte[] packet = _packetMaker.MakeGetGoldCheatPacket();
        if (packet == null) { return; }
        SendPacketWhenPlayingGame(packet);
    }
    public void SendGetDiaCheatPacket()
    {
        byte[] packet = _packetMaker.MakeGetDiaCheatPacket();
        if (packet == null) { return; }
        SendPacketWhenPlayingGame(packet);
    }
}
