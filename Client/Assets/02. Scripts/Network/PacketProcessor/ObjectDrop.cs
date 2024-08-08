using Google.FlatBuffers;
using PacketTable.ObjectTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Network.PacketProcessor
{
    public class ObjectDropProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data, GameObject playerManager)
        {
            var bb = new ByteBuffer(data);
            var Data = ObjectDrop.GetRootAsObjectDrop(bb);

            int posX = Data.Pos.Value.X; 
            int posY = Data.Pos.Value.Y;
            int objectID = Data.Id;


        }

        private FlatBufferBuilder mBuilder;
    }
}
