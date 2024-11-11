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

            if (Managers.Game.isInGame)
            {
                for (int i = 0; i < Data.PosLength; ++i)
                {
                    float posX = Data.Pos(i).Value.X;
                    float posZ = Data.Pos(i).Value.Z;
                    int id = Data.Id(i);
                    int explosion_delay = Data.ExplosionDelayTime(i);
                    // 폭탄 생성
                    Managers.BombObject.SpawnBomb(posX, posZ, id);
                }
            }
        }

        
    }
}
