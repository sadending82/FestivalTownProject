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
    public class GameMatchingResponseProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = GameMatchingResponse.GetRootAsGameMatchingResponse(bb);

            int id = Data.Ingameid;
            int roomID = Data.Roomid;
            bool isHost = Data.IsHost;

            Managers.Player.GetComponent<PlayerManager>().SetMyId(id);
            Managers.Player.GetComponent<PlayerManager>().SetIsHost(isHost);
            Managers.Network.GetPacketManager().SendGameReady();
        }

        private FlatBufferBuilder mBuilder;
    }
}
