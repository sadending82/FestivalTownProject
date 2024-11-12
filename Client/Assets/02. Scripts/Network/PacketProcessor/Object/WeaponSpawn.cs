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

            if (Managers.Game.isInGame)
            {
                for (int i = 0; i < Data.PosLength; ++i)
                {
                    float posX = Data.Pos(i).Value.X;
                    float posZ = Data.Pos(i).Value.Z;
                    int id = Data.Id(i);
                    eWeaponType type = (eWeaponType)Data.Type(i);

                    // 무기 생성
                    Managers.WeaponObject.SpawnWeapon(posX, posZ, type, id);
                }
            }
        }

        
    }
}
