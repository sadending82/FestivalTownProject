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
    public class PlayerGameInfoProcessor : PacketProcessor
    {
        public override void Process(byte[] data, GameObject testObject)
        {
            // 여기에 처리    
            var bb = new ByteBuffer(data);

            var Data = PlayerGameInfo.GetRootAsPlayerGameInfo(bb);

            int id = Data.Ingameid;
            int roomID = Data.Roomid;
        }

        private FlatBufferBuilder mBuilder;
    }
}
