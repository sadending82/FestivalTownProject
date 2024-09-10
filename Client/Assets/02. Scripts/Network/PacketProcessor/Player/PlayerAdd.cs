using Google.FlatBuffers;
using PacketTable.PlayerTable;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

namespace Network.PacketProcessor
{
    public class PlayerAddProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = PlayerAdd.GetRootAsPlayerAdd(bb);

            for (int i = 0; i < Data.PlayersLength; ++i)
            {
                var player = Data.Players(i);
                int id = player.Value.Id;
                Vector3 pos = new Vector3(player.Value.Pos.Value.X, player.Value.Pos.Value.Y, 30);
                Managers.Player.GetComponent<PlayerManager>().AddPlayer(id);
            }
        }

        private FlatBufferBuilder mBuilder;
    }
}
