using Google.FlatBuffers;
using PacketTable.PlayerTable;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Unity.VisualScripting.FullSerializer;
using UnityEngine;

namespace Network.PacketProcessor
{
    public class PlayerRespawnProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var Data = PlayerRespawn.GetRootAsPlayerRespawn(bb);

            float x = Data.Pos.Value.X;
            float z = Data.Pos.Value.Z;
            int targetId = Data.Id;

            PlayerController pCtr = Managers.Player.GetPlayers().transform.GetChild(targetId).GetComponent<PlayerController>();
            pCtr.Respawn(x, z);
        }

        
    }
}
