using Google.FlatBuffers;
using PacketTable.ObjectTable;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Unity.VisualScripting.FullSerializer;
using UnityEngine;

namespace Network.PacketProcessor
{
    public class PlayerDropWeaponProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = WeaponPosition.GetRootAsWeaponPosition(bb);
            int weaponID = Data.Id;
            Vector3 pos = new Vector3(Data.Pos.Value.X, Data.Pos.Value.Y, Data.Pos.Value.Z);

            Weapon targetWeapon = Managers.WeaponObject.FindWeaponById(weaponID).GetComponent<Weapon>();
            targetWeapon.Drop(pos);
        }

        
    }
}
