using Google.FlatBuffers;
using PacketTable.ObjectTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Network.PacketProcessor
{
    public class WeaponDeleteProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var Data = WeaponDelete.GetRootAsWeaponDelete(bb);
            int id = Data.Id;

            Weapon targetWeapon = Managers.WeaponObject.FindWeaponById(id).GetComponent<Weapon>();
            if (targetWeapon == null)
            {
                Debug.Log("지우려는데 웨폰이 없다데스!!!");
            }
            targetWeapon.DeleteWeapon();
        }
    }
}
