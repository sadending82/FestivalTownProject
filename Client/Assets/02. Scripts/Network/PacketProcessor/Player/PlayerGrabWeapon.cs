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
    public class PlayerGrabWeaponProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = PlayerGrabWeapon.GetRootAsPlayerGrabWeapon(bb);
            int playerID = Data.Id;
            Vector3 pos = new Vector3(Data.Pos.Value.X, Data.Pos.Value.Y, Data.Pos.Value.Z);
            Vector3 dir = new Vector3(Data.Direction.Value.X, Data.Direction.Value.Y, Data.Direction.Value.Z);
            int WeaponID = Data.WeaponId;

            PlayerController pController = Managers.Player.GetPlayers().transform.GetChild(playerID).GetComponent<PlayerController>();
            pController.SetDirection(dir);
            pController.SetPosition(pos);
            //pController.s_PickUpItem(playerID, WeaponID);
        }

        private FlatBufferBuilder mBuilder;
    }
}
