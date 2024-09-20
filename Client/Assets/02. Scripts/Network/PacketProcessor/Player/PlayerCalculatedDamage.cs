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
    public class PlayerCalculatedDamageProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var Data = PlayerCalculatedDamage.GetRootAsPlayerCalculatedDamage(bb);

            int targetId = Data.TargetId;
            Vector3 dir = new Vector3(Data.KnockbackDirection.Value.X, Data.KnockbackDirection.Value.Y, Data.KnockbackDirection.Value.Z);
            int hp = Data.Hp;

            Debug.Log("맞은 사람 ID : " + targetId);

            CharacterStatus pState = Managers.Player.GetPlayers().transform.GetChild(targetId).GetComponent<CharacterStatus>();
            pState.Attacked(dir);
            pState.SetHp(hp);
        }

        
    }
}
