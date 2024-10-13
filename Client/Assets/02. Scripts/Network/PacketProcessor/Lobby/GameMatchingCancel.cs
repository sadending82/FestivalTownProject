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

            if (Managers.Scene.CurrentScene.GetComponent<HomeScene>() != null)
            {
                Managers.Scene.CurrentScene.GetComponent<HomeScene>().isMatching = false;
            }

            Managers.UI.ClosePopUpUI();

        }

        
    }
}
