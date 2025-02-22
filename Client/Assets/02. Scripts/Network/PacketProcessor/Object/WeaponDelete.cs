﻿using Google.FlatBuffers;
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

            if (Managers.Game.isInGame && !Managers.Game.isGameEnd)
            {
                GameObject targetWeapon = Managers.WeaponObject.FindWeaponById(id);
                if (targetWeapon != null)
                {
                    targetWeapon.GetComponent<Weapon>().DeleteWeapon();
                }
            }
        }
    }
}
