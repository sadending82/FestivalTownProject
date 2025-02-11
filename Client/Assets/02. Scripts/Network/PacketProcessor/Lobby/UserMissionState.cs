using Google.FlatBuffers;
using NetworkProtocol;
using PacketTable.PassTable;

using System.Collections.Generic;

using UnityEngine;

namespace Network.PacketProcessor
{
    public class UserMissionStateProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = UserMissionStateList.GetRootAsUserMissionStateList(bb);

            Debug.Log($"recv mission");

            var MissionUI = Managers.UI.GetTopOfPopUPUI().GetComponent<UI_Mission>();
            if (MissionUI == null) return;

            for (int i = 0; i < Data.MissionStateListLength; ++i)
            {
                var missionState = Data.MissionStateList(i).Value;

                int mission_index = missionState.MissionIndex;
                int progress = missionState.Progress;
                bool is_completed = missionState.IsRewarded;

                Debug.Log($"recv {mission_index} mission Data. index : {mission_index}, progress : {progress}, is_completed : {is_completed}");

                MissionUI.AddMission(mission_index, progress, is_completed);
            }

        }
    }
}
