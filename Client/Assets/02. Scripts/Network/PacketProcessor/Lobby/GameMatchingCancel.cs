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
    public class GameMatchingCancelProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = GameMatchingCancel.GetRootAsGameMatchingCancel(bb);

            // 나중에 특정 UI를 지운다는 느낌으로 변경해야 합니다.
            // 이건 UIManager를 변경한 이후 할 예정입니다.
            Managers.UI.ClosePopUpUI();
        }

        private FlatBufferBuilder mBuilder;
    }
}
