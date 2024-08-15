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
    public class PlayerGrabBombProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = PlayerGrabBomb.GetRootAsPlayerGrabBomb(bb);
            int playerID = Data.Id;
            Vector3 pos = new Vector3(Data.Pos.Value.X, Data.Pos.Value.Y, Data.Pos.Value.Z);
            Vector3 dir = new Vector3(Data.Direction.Value.X, Data.Direction.Value.Y, Data.Direction.Value.Z);
            int bombID = Data.BombId;


        }

        private FlatBufferBuilder mBuilder;
    }
}
