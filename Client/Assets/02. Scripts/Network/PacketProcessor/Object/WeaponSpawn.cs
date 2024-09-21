using Google.FlatBuffers;
using PacketTable.ObjectTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using NetworkProtocol;


namespace Network.PacketProcessor
{
    public class WeaponSpawnProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var Data = WeaponSpawn.GetRootAsWeaponSpawn(bb);

            float posX = Data.Pos.Value.X;
            float posZ = Data.Pos.Value.Z;
            int id = Data.Id;
            eWeaponType type = (eWeaponType)Data.Type;

            // 무기 생성
            Managers.WeaponObject.SpawnWeapon(posX, posZ, type, id);
        }

        
    }
}
