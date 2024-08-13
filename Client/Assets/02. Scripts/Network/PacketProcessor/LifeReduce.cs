using Google.FlatBuffers;
using PacketTable.GameTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Network.PacketProcessor
{
    public class LifeReduceProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var Data = LifeReduce.GetRootAsLifeReduce(bb);

            int team = Data.Team;
            int lifeCount = Data.Life;

        }

        private FlatBufferBuilder mBuilder;
    }
}
