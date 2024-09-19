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

            for (int i = 0; i < Data.WiningTeamLength; ++i)
            {
                var winningTeam = Data.WiningTeam(i);
            }
            Managers.Game.SetWinningTeam(0);

            for (int i = 0; i < Data.PlayerRecordsLength; ++i){
                var playerRecord = Data.PlayerRecords(i);

                Debug.Log($"Player {playerRecord.Value.Id}:  Kill = {playerRecord.Value.KillCount}, Death = {playerRecord.Value.DeathCount}" +
                    $",  Gold = {playerRecord.Value.EarnGold}, Point = {playerRecord.Value.Point} Is MVP = {playerRecord.Value.IsMvp}");

                Define.PlayerResult result;
                result.kill = playerRecord.Value.KillCount;
                result.death = playerRecord.Value.DeathCount;
                result.bombInsert = playerRecord.Value.BombInsertCount;
                result.gold = playerRecord.Value.EarnGold;

                Managers.Game.PlayerResultData.Add(playerRecord.Value.Id, result);
            }

            

            if(Managers.Scene.CurrentScene.GetComponent<GameScene>() != null)
            {
                Managers.Scene.CurrentScene.GetComponent<GameScene>().WaitAndShowResult();
            }
        }

        private FlatBufferBuilder mBuilder;
    }
}
