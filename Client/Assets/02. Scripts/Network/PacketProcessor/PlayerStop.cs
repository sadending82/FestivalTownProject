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
    public class PlayerStopProcessor : PacketProcessor
    {
        public override void Process(byte[] data, GameObject playerManager)
        {
            // 여기에 처리    
            var bb = new ByteBuffer(data);
            var stopData = PlayerStop.GetRootAsPlayerStop(bb);

            int id = stopData.Id; // 이거는 일단 안씀
            Vector3 pos = new Vector3(stopData.Pos.Value.X, stopData.Pos.Value.Y, stopData.Pos.Value.Z);
            Vector3 dir = new Vector3(stopData.Direction.Value.X, stopData.Direction.Value.Y, stopData.Direction.Value.Z);
            if (playerManager != null) { return; }

            if (playerManager.transform.GetChild(id) != null)
            {
                PlayerController pController = playerManager.transform.GetChild(id).GetComponent<PlayerController>();
                if (pController != null)
                {
                    pController.SetDirection(dir);
                    pController.SetPosition(pos);
                    pController.SetIsMove(false);
                }
            }
        }

        private FlatBufferBuilder mBuilder;
    }
}
