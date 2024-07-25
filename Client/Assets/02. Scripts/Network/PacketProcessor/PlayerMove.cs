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
    public class PlayerMoveProcesser : PacketProcessor
    {
        public override void Process(byte[] data, GameObject testObject) 
        {
            // 여기에 처리    
            var bb = new ByteBuffer(data);
            var moveData = PlayerMove.GetRootAsPlayerMove(bb);

            int key = moveData.Key;// 이거는 일단 안씀
            Vector3 pos = new Vector3(moveData.Pos.Value.X, moveData.Pos.Value.Y, moveData.Pos.Value.Z);
            Vector3 dir = new Vector3(moveData.Direction.Value.X, moveData.Direction.Value.Y, moveData.Direction.Value.Z);

            Debug.Log("move pos: " + pos.x + " " + pos.y + " " + pos.z);

            testObject.GetComponent<TestReceive>().SetDirection(dir);
            testObject.GetComponent<TestReceive>().SetPosition(pos);
            testObject.GetComponent<TestReceive>().SetIsMove(true);
        }

        private FlatBufferBuilder mBuilder;
    }
}
