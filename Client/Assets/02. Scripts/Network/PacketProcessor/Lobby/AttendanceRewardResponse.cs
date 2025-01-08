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

            int eventItemListIdx = 100000 + eventCode + daycount - 1;

            if (Managers.Data.AttendanceEventDataDict.TryGetValue(eventItemListIdx, out var attendanceData))
            {
                Debug.Log($"{daycount}일차 아이템 받앗숴요.");
                attendanceData.isRewarded = true;
                Managers.Data.AttendanceEventDataDict[eventItemListIdx] = attendanceData;

                Managers.Network.GetPacketManager().SendCurrencyAmountRequestPacket();
                Managers.Network.GetPacketManager().SendUserItemsRequestPacket();

                switch (eventCode)
                {
                    case 32001:
                        {
                            var ui = Managers.UI.GetTopOfPopUPUI().GetComponent<UI_DailySignInCheck>();
                            ui.UpdateItemCheck();
                        }
                        break;
                    case 31001:
                        {
                            var ui = Managers.UI.GetTopOfPopUPUI().GetComponent<UI_DevelopersThanks>();
                            ui.UpdateItemCheck();
                        }
                        break;
                }   
                if(Managers.Scene.CurrentScene.GetComponent<HomeScene>() != null)
                {
                    Managers.UI.GetCurrentSceneUI().GetComponent<UI_HomeStart>().CheckAttendanceEventData();
                }
            }
        }


    }
}
