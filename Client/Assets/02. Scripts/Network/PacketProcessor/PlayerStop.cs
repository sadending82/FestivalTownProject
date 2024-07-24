using Google.FlatBuffers;
using PacketTable.Player;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

namespace Network.PacketProcessor
{
    public class PlayerStopProcesser : PacketProcessor
    {
        public override void Process(byte[] data)
        {
            // 여기에 처리    
            var bb = new ByteBuffer(data);
            var stopData = PlayerStop.GetRootAsPlayerStop(bb);

            int key = stopData.Key; // 이거는 일단 안씀
            Vector3 pos = new Vector3(stopData.Pos.Value.X, stopData.Pos.Value.Y, stopData.Pos.Value.Z);
            Vector3 dir = new Vector3(stopData.Direction.Value.X, stopData.Direction.Value.Y, stopData.Direction.Value.Z);

            Debug.Log("pos: " + pos.x + " " + pos.y + " " + pos.z);

            GameObject testObject = GameObject.FindWithTag("Test");
            testObject.GetComponent<TestReceive>().SetDirection(dir);
            testObject.GetComponent<TestReceive>().SetPosition(pos);
            testObject.GetComponent<TestReceive>().SetIsMove(false);
        }

        private FlatBufferBuilder mBuilder;

    }
}
