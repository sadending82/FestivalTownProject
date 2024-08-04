using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace NetworkProtocol
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    struct HEADER
    {
        [MarshalAs(UnmanagedType.U2)]
        public ushort size;
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

        // Client -> Server
        C2S_HEARTBEAT,
        C2S_PLAYERMOVE,
        C2S_PLAYERSTOP,
        C2S_PLAYERPOSSYNC
    };

    public enum ePlayerState
    {
        PS_RUN,
        PS_JUMP,
        PS_MOVESTOP,
        PS_JUMPSTOP
    };
}
