using Google.FlatBuffers;
using NetworkProtocol;
using PacketTable.PassTable;

using System.Collections.Generic;

using UnityEngine;

namespace Network.PacketProcessor
{
    public class UserPassStateProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = UserPassState.GetRootAsUserPassState(bb);

            int pass_index = Data.PassIndex;
            int pass_type = Data.PassType;
            int pass_level = Data.PassLevel;
            int pass_exp = Data.PassExp;



            var ui = Managers.UI.GetCurrentSceneUI().GetComponent<UI_Pass>();
            if (ui != null) return;

            Debug.Log($"Pass Data Recved. {pass_index}, {pass_type}, {pass_level}, {pass_exp}");

            ui.SetLevel(pass_level);
            ui.SetProgress(pass_exp, 100);

            for (int i = 0; i < Data.PassRewardStateLength; ++i)
            {
                var passRewardState = Data.PassRewardState(i).Value;

                int rewardType = passRewardState.PassType;
                int rewardLevel = passRewardState.Level;
                bool isRewarded = passRewardState.IsRewarded;

                ui.SetPassItemRewarded(pass_index, isRewarded);

            }

        }
    }
}
