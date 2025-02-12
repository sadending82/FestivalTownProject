using Google.FlatBuffers;
using NetworkProtocol;
using PacketTable.PassTable;

using System.Collections.Generic;

using UnityEngine;

namespace Network.PacketProcessor
{
    public class BatchReceivePassRewardsResponseProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = BatchReceivePassRewardsResponse.GetRootAsBatchReceivePassRewardsResponse(bb);

            for (int i = 0; i < Data.RewardsLength; i++)
            {
                var reward = Data.Rewards(i).Value;

                int result = reward.Result;

                if (result == 0)
                {

                }
                else if (result == 1)
                {
                    int pass_index = reward.PassIndex;
                    int pass_type = reward.PassType;
                    int pass_level = reward.PassLevel;

                    int reward_item_index = reward.RewardItemIndex;
                    int reward_item_amount = reward.RewardItemAmount;

                    var passUI = Managers.UI.GetCurrentSceneUI().GetComponent<UI_Pass>();
                    if (passUI == null) return;

                    passUI.SetPassItemRewarded(pass_index * 100 + ((pass_type - 1) * 100) + pass_level, true);
                    Debug.Log($"Pass Reward Recv. index : {pass_index}, type : {pass_type}, level : {pass_level}");
                }
            }

            Managers.Network.GetPacketManager().SendCurrencyAmountRequestPacket();
        }
    }
}
