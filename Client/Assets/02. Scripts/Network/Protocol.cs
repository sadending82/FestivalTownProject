using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace NetworkProtocol
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
    struct HEADER
    {
        [MarshalAs(UnmanagedType.U2)]
        public ushort flatBufferSize;
        [MarshalAs(UnmanagedType.U2)]
        public ushort type;
    }

    public enum ePacketType
    {
        // Server -> Client
        S2C_HEART_BEAT,
        S2C_PLAYER_ADD,
        S2C_PLAYER_MOVE,
        S2C_PLAYER_STOP,
        S2C_PLAYER_POS_SYNC,
        S2C_PLAYER_GRAB_BOMB,
        S2C_PLAYER_THROW_BOMB,
        S2C_PLAYER_GRAB_WEAPON,
        S2C_PLAYER_DROP_WEAPON,
        S2C_PLAYER_ANIMATION,
        S2C_PLAYER_DAMAGE_RECEIVE,
        S2C_PLAYER_CALCULATED_DAMAGE,
        S2C_PLAYER_DEAD,
        S2C_PLAYER_RESPAWN,

        S2C_BLOCK_DROP,
        S2C_BOMB_SPAWN,
        S2C_BOMB_EXPLOSION,
        S2C_BOMB_POS_SYNC,
        S2C_WEAPON_SPAWN,
        S2C_WEAPON_DELETE,

        S2C_MATCHING_RESPONSE,
        S2C_MATCHING_CANCEL,
        S2C_ALL_PLAYER_READY,
        S2C_GAME_START,
        S2C_GAME_END,
        S2C_GAME_HOST_CHANGE,

        S2C_LIFE_REDUCE,
        S2C_REMAIN_TIME_SYNC,

        // Client -> Server
        C2S_HEART_BEAT,
        C2S_PLAYER_MOVE,
        C2S_PLAYER_STOP,
        C2S_PLAYER_POS_SYNC,
        C2S_PLAYER_GRAB_BOMB,
        C2S_PLAYER_THROW_BOMB,
        C2S_PLAYER_GRAB_WEAPON,
        C2S_PLAYER_DROP_WEAPON,
        C2S_PLAYER_ANIMATION,
        C2S_PLAYER_DAMAGE_RECEIVE,

        C2S_BOMB_INPUT,
        C2S_BOMB_POS_SYNC,
        C2S_BOMB_EXPLOSION,
        C2S_WEAPON_DELETE,

        C2S_MATCHING_REQUEST,
        C2S_MATCHING_CANCEL,
        C2S_GAME_READY
    };

    public enum GameCode
    {
        FITH_Team_battle_One,
        FITH_Team_battle_Two,
        FITH_Team_battle_Three,
    };

    public enum ePlayerMoveState
    {
        PS_WALK,
        PS_RUN,
        PS_JUMP,
        PS_MOVESTOP,
        PS_JUMPSTOP
    };

    public enum eWeaponType
    {
        WT_HAND,
        WT_FRYING_PAN,
        WT_BAT
    };

    public enum eAttackType
    {
        AT_FALLDOWN,
        AT_BASIC
    };
}

namespace ClientProtocol
{
    public enum UpperBodyAnimationState
    {
        NONE,
        ATTACK, POWERATTACK, HOLD, THROW, HEADATTACK, ROLL, FLYINGKICK
    }

    // 서버에 전달 X 클라가 관리
    public enum LowerBodyAnimationState
    {
        IDLE, WALK, RUN
    }
}