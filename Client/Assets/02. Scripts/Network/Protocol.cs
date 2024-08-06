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
        public ushort size;
        [MarshalAs(UnmanagedType.U2)]
        public ushort type;
    }


    static class ePacketType
    {
        // Server -> Client
        public const int S2C_HEARTBEAT = 0;
        public const int S2C_PLAYERADD = 1;
        public const int S2C_PLAYERGAMEINFO = 2;
        public const int S2C_PLAYERMOVE = 3;
        public const int S2C_PLAYERSTOP = 4;
        public const int S2C_PLAYERPOSSYNC = 5;

        // Client -> Server
        public const int C2S_HEARTBEAT = 100;
        public const int C2S_PLAYERMOVE = 101;
        public const int C2S_PLAYERSTOP = 102;
        public const int C2S_PLAYERPOSSYNC = 103;
    }

    public enum ePlayerState
    {
        PS_RUN,
        PS_JUMP,
        PS_MOVESTOP,
        PS_JUMPSTOP
    };
}
