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

            for (int i = 0; i < Data.MissionStateListLength; ++i)
            {
                var missionState = Data.MissionStateList(i).Value;

                int mission_index = missionState.MissionIndex;
                int progress = missionState.Progress;
                bool is_completed = missionState.IsRewarded;
            }

        }
    }
}
