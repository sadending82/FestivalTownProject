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
        S2C_HEARTBEAT,
        S2C_PLAYERADD,
        S2C_PLAYERMOVE,
        S2C_PLAYERSTOP,
        S2C_PLAYERPOSSYNC,
        S2C_PLAYERGRABBOMB,
        S2C_PLAYERTHROWBOMB,
        S2C_PLAYERANIMATION,
        S2C_PLAYERDAMAGERECEIVE,
        S2C_PLAYERCALCULATEDDAMAGE,
        S2C_PLAYERDEAD,
        S2C_PLAYERRESPAWN,

        S2C_BLOCKDROP,
        S2C_BOMBSPAWN,
        S2C_BOMBEXPLOSION,
        S2C_BOMBPOSSYNC,

        S2C_MATCHINGRESPONSE,
        S2C_ALLPLAYERREADY,
        S2C_GAMESTART,
        S2C_GAMEEND,
        S2C_GAMEHOSTCHANGE,

        S2C_LIFEREDUCE,
        S2C_REMAINTIMESYNC,

        // Client -> Server
        C2S_HEARTBEAT,
        C2S_PLAYERMOVE,
        C2S_PLAYERSTOP,
        C2S_PLAYERPOSSYNC,
        C2S_PLAYERGRABBOMB,
        C2S_PLAYERTHROWBOMB,
        C2S_PLAYERANIMATION,
        C2S_PLAYERDAMAGERECEIVE,

        C2S_BOMBINPUT,
        C2S_BOMBPOSSYNC,
        C2S_BOMBEXPLOSION,

        C2S_MATCHINGREQUEST,
        C2S_GAMEREADY
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
        WT_HAND
    };

    public enum eAttackType
    {
        AT_FALLDOWN,
        AT_BASIC
    };
}
