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
    public class PlayerDeadProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var Data = PlayerDead.GetRootAsPlayerDead(bb);

            int targetId = Data.Id;
            CharacterStatus pState = Managers.Player.GetPlayers().transform.GetChild(targetId).GetComponent<CharacterStatus>();
            pState.SetIsDie(true);
        }

        
    }
}
