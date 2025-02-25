using Google.FlatBuffers;
using NetworkProtocol;
using PacketTable.PassTable;
using System;
using System.Collections.Generic;

using UnityEngine;

namespace Network.PacketProcessor
{
    public class MissionCompleteResponseProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = MissionCompleteResponse.GetRootAsMissionCompleteResponse(bb);

            int result = Data.Result;



            if (result == 0)
            {

            }
            else if (result == 1)
            {
                int mission_index = Data.MissionIndex;

                int pass_index = Data.PassIndex;
                int pass_level = Data.PassLevel;
                int pass_exp = Data.PassExp;
                int daily_mission_exp = Data.DailyMissionExp;

                var PassUI = Managers.UI.GetCurrentSceneUI().GetComponent<UI_Pass>();
                if (PassUI != null)
                {
                    Managers.Data.SetPassLevel(pass_level);
                    Managers.Data.SetPassExp(pass_exp);
                    PassUI.SetLevel(pass_level);
                    PassUI.SetProgress(pass_exp, 100);
                }

                int reward_item_index = Data.RewardItemIndex;
                int reward_item_amount = Data.RewardItemAmount;

                Int64 complete_time = Data.CompleteTime;

                Debug.Log($"recv {mission_index} mission Complete Response");

                var MissionUI = Managers.UI.GetTopOfPopUPUI().GetComponent<UI_Mission>();
                if (MissionUI == null) return;

                var mdUi = MissionUI.ChangeMissionData(mission_index, Managers.Data.PassMissionDataDict[mission_index].Required_Count, true);
                MissionUI.SetRecentPosition(mdUi);

                switch (Managers.Data.GetResourceIndexType(reward_item_index))
                {
                    case DataManager.ResourceIndexType.Gold:
                        int currGold = Managers.Data.GetGold();
                        Managers.Data.SetGold(currGold + reward_item_amount);
                        break;
                    case DataManager.ResourceIndexType.Diamond:
                        int currDiamond = Managers.Data.GetDiamond();
                        Managers.Data.SetDiamond(currDiamond + reward_item_amount);
                        break;
                    case DataManager.ResourceIndexType.Mileage:
                        int currMileage = Managers.Data.GetMileage();
                        Managers.Data.SetMileage(currMileage + reward_item_amount);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
