using Google.FlatBuffers;
using PacketTable.GameTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Network.PacketProcessor
{
    public class RemainTimeSyncProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var Data = RemainTimeSync.GetRootAsRemainTimeSync(bb);

            int remainTime = Data.RemainTime; // Seconds
        }

        private FlatBufferBuilder mBuilder;
    }
}
