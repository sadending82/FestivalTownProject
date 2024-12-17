using Google.FlatBuffers;
using PacketTable.PlayerTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Network.PacketProcessor
{
    public class PlayerSyncProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var posData = PlayerSync.GetRootAsPlayerSync(bb);

            int id = posData.Id;
            Vector3 pos = new Vector3(posData.Pos.Value.X, posData.Pos.Value.Y, posData.Pos.Value.Z);
            Vector3 dir = new Vector3(posData.Direction.Value.X, posData.Direction.Value.Y, posData.Direction.Value.Z);

            //Debug.Log("Player ID : " + id + ", PlayerSyncPacket");

            if (Managers.Player == null) return;

            if (Managers.Player.GetPlayers().transform.GetChild(id) != null)
            {
                PlayerController pController = Managers.Player.GetPlayers().transform.GetChild(id).GetComponent<PlayerController>();

                if (pController.GetIsMove() == true)
                {
                    pController.SetPosition(pos);
                    pController.SetDirection(dir);
                }
            }
        }

        
    }
}
