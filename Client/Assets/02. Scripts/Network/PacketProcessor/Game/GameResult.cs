using Google.FlatBuffers;
using PacketTable.GameTable;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.WebSockets;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

namespace Network.PacketProcessor
{
    public class GameResultProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = GameResult.GetRootAsGameResult(bb);

            var winingTeam_Flag = Data.WinningteamsFlag;

            for (int i = 0; i < Data.PlayerRecordsLength; ++i){
                var playerRecord = Data.PlayerRecords(i);

                Debug.Log($"Player {playerRecord.Value.Id}:  Kill = {playerRecord.Value.KillCount}, Death = {playerRecord.Value.DeathCount}" +
                    $",  Gold = {playerRecord.Value.EarnGold}, Point = {playerRecord.Value.Point} Is MVP = {playerRecord.Value.IsMvp}");
            }
        }

        private FlatBufferBuilder mBuilder;
    }
}
