using Google.FlatBuffers;
using PacketTable.PlayerTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Network.PacketProcessor
{
    public class PlayerPosProcessor : PacketProcessor
    {
        public override void Process(byte[] data, GameObject playerManager)
        {
            var bb = new ByteBuffer(data);
            var posData = PlayerPos.GetRootAsPlayerPos(bb);

            int id = posData.Id;
            Vector3 pos = new Vector3(posData.Pos.Value.X, posData.Pos.Value.Y, posData.Pos.Value.Z);
            Vector3 dir = new Vector3(posData.Direction.Value.X, posData.Direction.Value.Y, posData.Direction.Value.Z);

            playerManager.transform.GetChild(id).GetComponent<PlayerController>().SetDirection(dir);
            playerManager.transform.GetChild(id).GetComponent<PlayerController>().SetPosition(pos);

        }

        private FlatBufferBuilder mBuilder;
    }
}