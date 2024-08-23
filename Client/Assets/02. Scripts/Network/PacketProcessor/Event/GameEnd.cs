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
    public class GameEndProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = GameEnd.GetRootAsGameEnd(bb);

            byte flag = Data.WinningteamsFlag;

            Managers.Scene.LoadScene(Define.Scene.Home);
        }

        private FlatBufferBuilder mBuilder;
    }
}
