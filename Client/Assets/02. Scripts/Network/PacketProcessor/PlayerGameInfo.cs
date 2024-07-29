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
            Debug.Log("GameInfo 받음");
            // 여기에 처리    
            var bb = new ByteBuffer(data);
            
        }

        private FlatBufferBuilder mBuilder;
    }
}
