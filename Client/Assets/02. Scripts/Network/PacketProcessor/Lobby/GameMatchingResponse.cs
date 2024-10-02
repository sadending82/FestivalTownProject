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
            int gameTime = Data.GameTime; // Seconds

            Debug.Log("Game Maching Complete");

            Managers.Scene.LoadScene(Define.Scene.Loading);
            Managers.Game.RoomID = roomID;
            Managers.Game.inGameID = id;
            Managers.Game.isHost = isHost;
            Managers.Game.GameRemainTime = gameTime;
        }

        
    }
}
