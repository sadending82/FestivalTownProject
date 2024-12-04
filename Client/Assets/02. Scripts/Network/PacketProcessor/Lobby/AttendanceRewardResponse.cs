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
    public class AttendanceRewardResponseProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = AttendanceRewardResponse.GetRootAsAttendanceRewardResponse(bb);

            int eventCode = Data.EventCode;
            int daycount = Data.DayCount;
            int result = Data.Result;
            int reward_item = Data.RewardItem;
            int reward_item_count = Data.RewardItemCount;

            if (Managers.Data.AttendanceEventDataDict.TryGetValue(daycount, out var attendanceData))
            {
                Debug.Log($"{daycount}일차 아이템 받앗숴요.");
                attendanceData.isRewarded = true;
                Managers.Data.AttendanceEventDataDict[daycount] = attendanceData;
                var ui = Managers.UI.GetTopOfPopUPUI().GetComponent<UI_DailySignInCheck>();
                Managers.Network.GetPacketManager().SendCurrencyAmountRequestPacket();
                Managers.Network.GetPacketManager().SendUserItemsRequestPacket();
                ui.UpdateItemCheck();
                
            }
        }


    }
}
