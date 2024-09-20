using Google.FlatBuffers;
using PacketTable.GameTable;
using PacketTable.LobbyTable;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

namespace Network.PacketProcessor
{
    public class GameHostChangeProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {   
            var bb = new ByteBuffer(data);

            var Data = GameHostChange.GetRootAsGameHostChange(bb);

            Managers.Player.GetComponent<PlayerManager>().SetIsHost(true);

            Debug.Log("change host!!");
        }

        
    }
}
