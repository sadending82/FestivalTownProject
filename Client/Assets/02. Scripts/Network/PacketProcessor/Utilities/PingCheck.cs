using Google.FlatBuffers;
using PacketTable.UtilitiesTable;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Network.PacketProcessor
{
    public class PingCheckProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            long currTime = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();

            long pingTime = currTime - Managers.Network.GetPacketManager().pingStartTime;

            Debug.Log($"Ping: {pingTime}m/s");
        }


    }
}
