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
        PACKET_TYPE_NONE,
        S2C_VERSION_CHECK,
        S2C_LOGIN_RESPONSE,
        S2C_SIGNUP_RESPONSE,

        S2C_HEART_BEAT,
        S2C_PLAYER_ADD,
        S2C_PLAYER_DELETE,
        S2C_PLAYER_MOVE,
        S2C_PLAYER_STOP,
        S2C_PLAYER_SYNC,
        S2C_PLAYER_GRAB_BOMB,
        S2C_PLAYER_DROP_BOMB,
        S2C_PLAYER_THROW_BOMB,
        S2C_PLAYER_GRAB_WEAPON,
        S2C_PLAYER_DROP_WEAPON,
        S2C_PLAYER_ANIMATION,
        S2C_PLAYER_DAMAGE_RECEIVE,
        S2C_PLAYER_CALCULATED_DAMAGE,
        S2C_PLAYER_DEAD,
        S2C_PLAYER_RESPAWN,
        S2C_PLAYER_GROGGY,
        S2C_PLAYER_GROGGY_RECOVERY,
        S2C_PLAYER_GRAB_OTHER_PLAYER,
        S2C_PLAYER_THROW_OTHER_PLAYER,
        S2C_PLAYER_DASH,
        S2C_PLAYER_FLYING_KICK,

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
        S2C_GAME_RESULT,
        S2C_GAME_HOST_CHANGE,

        S2C_LIFE_REDUCE,
        S2C_REMAIN_TIME_SYNC,

        // Client -> Server
        C2S_VERSION_CHECK,
        C2S_LOGIN_REQUEST,
        C2S_SIGNUP_REQUEST,

        C2S_HEART_BEAT,
        C2S_PLAYER_MOVE,
        C2S_PLAYER_STOP,
        C2S_PLAYER_SYNC,
        C2S_PLAYER_GRAB_BOMB,
        C2S_PLAYER_DROP_BOMB,
        C2S_PLAYER_THROW_BOMB,
        C2S_PLAYER_GRAB_WEAPON,
        C2S_PLAYER_DROP_WEAPON,
        C2S_PLAYER_ANIMATION,
        C2S_PLAYER_DAMAGE_RECEIVE,
        C2S_PLAYER_COLLISION_BLOCK,
        C2S_PLAYER_GRAB_OTHER_PLAYER,
        C2S_PLAYER_THROW_OTHER_PLAYER,
        C2S_PLAYER_DASH,
        C2S_PLAYER_FLYING_KICK,

        C2S_BOMB_INPUT,
        C2S_BOMB_POS_SYNC,
        C2S_BOMB_EXPLOSION,
        C2S_WEAPON_DELETE,

        C2S_MATCHING_REQUEST,
        C2S_MATCHING_CANCEL,
        C2S_GAME_READY
    };

    public enum GameMode
    {
        INVALUE_MODE = 0,
        FITH_Indiv_Battle_2 = 111,
        FITH_Indiv_Battle_3 = 112,
        FITH_Indiv_Battle_5 = 113,
        FITH_Team_Battle_4 = 121,
        FITH_Team_Battle_6 = 122
    };
    public enum eMatchingType
    {
        FITH_SOLO = 110,
        FITH_TEAM = 120,
        FITH_TEST = 190
    };

    public enum ePlayerMoveState
    {
        PS_WALK,
        PS_RUN,
        PS_JUMP,
        PS_DASH,
        PS_FLYING_KICK,
        PS_MOVESTOP,
        PS_JUMPSTOP
    };

    public enum eWeaponType
    {
        WT_HAND,
        WT_FRYING_PAN = 2001,
        WT_BAT = 2002
    };

    public enum eBlockType
    {
        BT_BLOCK_2_2_2 = 3001,
        BT_BLOCK_2_2_1 = 3002,
    };


    public enum eDamageType
    {
        AT_FALLDOWN,
        AT_BOMB_ATTACK,
        AT_ATTACK = 10001
    };
}

namespace ClientProtocol
{
    public enum UpperBodyAnimationState
    {
        NONE,
        GRAP, THROW,
        ATTACK, ROLL, FLYINGKICK, WEAPONATTACK
    }

    // 서버에 전달 X 클라가 관리
    public enum LowerBodyAnimationState
    {
        IDLE, WALK, RUN
    }
}