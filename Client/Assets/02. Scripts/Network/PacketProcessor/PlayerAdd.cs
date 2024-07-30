using Google.FlatBuffers;
using PacketTable.PlayerTable;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

namespace Network.PacketProcessor
{
    public class PlayerAddProcessor : PacketProcessor
    {
        public override void Process(byte[] data, GameObject testObject)
        {
            // 여기에 처리    
            var bb = new ByteBuffer(data);

            var Data = PlayerAdd.GetRootAsPlayerAdd(bb);

            int id = Data.Id;
            Vector3 pos = new Vector3(Data.Pos.Value.X, Data.Pos.Value.Y, Data.Pos.Value.Z);
            Vector3 dir = new Vector3(Data.Direction.Value.X, Data.Direction.Value.Y, Data.Direction.Value.Z);

        }

        private FlatBufferBuilder mBuilder;
    }
}
