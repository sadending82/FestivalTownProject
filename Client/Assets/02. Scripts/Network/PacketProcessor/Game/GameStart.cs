using Google.FlatBuffers;
using PacketTable.GameTable;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

namespace Network.PacketProcessor
{
    public class GameStartProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = GameStart.GetRootAsGameStart(bb);

            Debug.Log("GameStartPacket");
            Managers.Game.isTimerStart = true;
        }

        private FlatBufferBuilder mBuilder;
    }
}
