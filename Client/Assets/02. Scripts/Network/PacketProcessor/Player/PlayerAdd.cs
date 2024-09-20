using Google.FlatBuffers;
using PacketTable.PlayerTable;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;
using static UnityEngine.GraphicsBuffer;

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
                int team = player.Value.Team;
                int characterType = player.Value.CharacterType;
                string nickname = player.Value.Nickname;

                Vector3 pos = new Vector3(player.Value.Pos.Value.X, player.Value.Pos.Value.Y, player.Value.Pos.Value.Z);
                Managers.Player.GetComponent<PlayerManager>().AddPlayer(id, pos, team);
            }
        }

        
    }
}
