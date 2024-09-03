using Google.FlatBuffers;
using PacketTable.ObjectTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Network.PacketProcessor
{
    public class WeaponSpawnProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var Data = WeaponSpawn.GetRootAsWeaponSpawn(bb);

            float posX = Data.Pos.Value.X;
            float posY = Data.Pos.Value.Y;
            int id = Data.Id;

        }

        private FlatBufferBuilder mBuilder;
    }
}
