using Google.FlatBuffers;
using PacketTable.UtilityTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Network.PacketProcessor
{
    public class HeartBeatProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data, GameObject playerManager)
        {
            var bb = new ByteBuffer(data);
            var Data = HeartBeat.GetRootAsHeartBeat(bb);

            //int id = Data.Sessionid;

            packetmanager.SendHeartBeatPacket();

        }

        private FlatBufferBuilder mBuilder;
    }
}
