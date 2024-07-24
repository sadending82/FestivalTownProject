using Google.FlatBuffers;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Google.FlatBuffers;
using UnityEngine;

namespace Network.PacketProcessor
{
    public class PlayerMoveProcesser : PacketProcessor
    {
        public override void Process(byte[] data) 
        {
            // 여기에 처리    

            Debug.Log("MOVE 옴?");
        }

        private FlatBufferBuilder mBuilder;
    }
}
