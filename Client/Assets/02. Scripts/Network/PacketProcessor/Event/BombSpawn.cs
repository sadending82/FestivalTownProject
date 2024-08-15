using Google.FlatBuffers;
using PacketTable.ObjectTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Network.PacketProcessor
{
    public class BombSpawnProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var Data = BombSpawn.GetRootAsBombSpawn(bb);

            int posX = Data.Pos.Value.X;
            int posY = Data.Pos.Value.Y;

            // 폭탄 생성
            Managers.BombObject.SpawnBomb(posX, posY);
        }

        private FlatBufferBuilder mBuilder;
    }
}
