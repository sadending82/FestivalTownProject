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
        S2C_PLAYERGAMEINFO,
        S2C_PLAYERMOVE,
        S2C_PLAYERSTOP,
        S2C_PLAYERPOSSYNC,
        S2C_PLAYERGRABBOMB,
        S2C_PLAYERTHROWBOMB,

        S2C_BLOCKDROP,
        S2C_BOMBSPAWN,
        S2C_BOMBPOSSYNC,

        S2C_LIFEREDUCE,
        S2C_REMAINTIMESYNC,

        // Client -> Server
        C2S_HEARTBEAT,
        C2S_PLAYERMOVE,
        C2S_PLAYERSTOP,
        C2S_PLAYERPOSSYNC,
        C2S_PLAYERGRABBOMB,
        C2S_PLAYERTHROWBOMB,

        C2S_BOMBINPUT,
        C2S_BOMBPOSSYNC

    };

    public enum ePlayerState
    {
        PS_RUN,
        PS_JUMP,
        PS_MOVESTOP,
        PS_JUMPSTOP
    };
}
