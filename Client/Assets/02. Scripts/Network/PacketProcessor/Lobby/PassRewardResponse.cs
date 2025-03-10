﻿using Google.FlatBuffers;
using NetworkProtocol;
using PacketTable.PassTable;

using System.Collections.Generic;

using UnityEngine;

namespace Network.PacketProcessor
{
    public class PassRewardResponseProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = PassRewardResponse.GetRootAsPassRewardResponse(bb);

            int result = Data.Result;

            if (result == 0)
            {

            }
            else if (result == 1)
            {
                int pass_index = Data.PassIndex;
                int pass_type = Data.PassType;
                int pass_level = Data.PassLevel;

                int reward_item_index = Data.RewardItemIndex;
                int reward_item_amount = Data.RewardItemAmount;

                var passUI = Managers.UI.GetCurrentSceneUI().GetComponent<UI_Pass>();
                if (passUI == null) return;

                passUI.SetPassItemRewarded(pass_index * 100 + ((pass_type - 1) * 100) + pass_level, true);
                Debug.Log($"Pass Reward Recv. index : {pass_index}, type : {pass_type}, level : {pass_level}");

                Managers.Network.GetPacketManager().SendCurrencyAmountRequestPacket();

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
