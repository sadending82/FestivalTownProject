using Google.FlatBuffers;
using PacketTable.PlayerTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Network.PacketProcessor
{
    public class PlayerGroggyProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var Data = PlayerGroggy.GetRootAsPlayerGroggy(bb);

            int id = Data.Id;

            // 그로기 처리


        }

        private FlatBufferBuilder mBuilder;
    }
}
