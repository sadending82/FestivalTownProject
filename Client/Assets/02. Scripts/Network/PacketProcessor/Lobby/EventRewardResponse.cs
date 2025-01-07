using ExcelDataStructure;
using Google.FlatBuffers;
using PacketTable.LobbyTable;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Unity.VisualScripting;
using UnityEngine;

namespace Network.PacketProcessor
{
    public class EventRewardResponseProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = EventRewardResponse.GetRootAsEventRewardResponse(bb);

            int eventCode = Data.EventCode;
            int result = Data.Result;
            int reward_item = Data.RewardItem;
            int reward_item_count = Data.RewardItemCount;

        }


    }
}
