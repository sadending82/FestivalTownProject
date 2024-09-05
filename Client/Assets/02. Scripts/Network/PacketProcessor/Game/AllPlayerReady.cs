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
    public class AllPlayerReadyProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = AllPlayerReady.GetRootAsAllPlayerReady(bb);

            Debug.Log("All Player Ready!");
            Managers.Scene.CurrentScene.GetComponent<LoadingScene>().ChangeNextScene();
        }

        private FlatBufferBuilder mBuilder;
    }
}
