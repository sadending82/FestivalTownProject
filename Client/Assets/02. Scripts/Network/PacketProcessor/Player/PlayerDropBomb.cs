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
    public class PlayerDropBombProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = PlayerDropBomb.GetRootAsPlayerDropBomb(bb);
            int playerID = Data.Id;
            if (Data.Pos.HasValue == false) return;
            Vector3 pos = new Vector3(Data.Pos.Value.X, Data.Pos.Value.Y, Data.Pos.Value.Z);
            int bombID = Data.BombId;

            GameObject go = Managers.BombObject.FindBombById(bombID);
            if (go == null) return;

            Bomb targetBomb = go.GetComponent<Bomb>();
            if (targetBomb != null)
            {
                targetBomb.Drop(pos);
            }
        }
    }
}
