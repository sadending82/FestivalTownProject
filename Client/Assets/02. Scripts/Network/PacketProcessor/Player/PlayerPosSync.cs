using Google.FlatBuffers;
using PacketTable.PlayerTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Network.PacketProcessor
{
    public class PlayerPosSyncProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var posData = PlayerPosSync.GetRootAsPlayerPosSync(bb);

            int id = posData.Id;
            Vector3 pos = new Vector3(posData.Pos.Value.X, posData.Pos.Value.Y, posData.Pos.Value.Z);
            Vector3 dir = new Vector3(posData.Direction.Value.X, posData.Direction.Value.Y, posData.Direction.Value.Z);

            Debug.Log("Player ID : " + id + ", PlayerPosSyncPacket");

            if (Managers.Player.transform.GetChild(id) != null)
            {
                Managers.Player.transform.GetChild(id).GetComponent<PlayerController>().SetDirection(dir);
                Managers.Player.transform.GetChild(id).GetComponent<PlayerController>().SetPosition(pos);
            }
        }

        private FlatBufferBuilder mBuilder;
    }
}
