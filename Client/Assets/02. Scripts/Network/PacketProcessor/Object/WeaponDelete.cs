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
            targetWeapon.DeleteWeapon();
            Debug.Log("Delete Weapon ID : " + id);
        }

        private FlatBufferBuilder mBuilder;
    }
}
