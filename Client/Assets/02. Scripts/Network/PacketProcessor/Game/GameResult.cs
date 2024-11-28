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

            int winningTeam = 0;
            for (int i = 0; i < Data.WiningTeamLength; ++i)
            {
                winningTeam = Data.WiningTeam(i);
            }
            Managers.Game.SetWinningTeam(winningTeam);

            for (int i = 0; i < Data.PlayerRecordsLength; ++i){
                var playerRecord = Data.PlayerRecords(i).Value;

                Debug.Log($"Player {playerRecord.Id}:  Kill = {playerRecord.KillCount}, Death = {playerRecord.DeathCount}" +
                    $",  Gold = {playerRecord.EarnGold}, Point = {playerRecord.Point} Is MVP = {playerRecord.IsMvp}");

                Define.PlayerResult result;
                result.Id = playerRecord.Id;
                result.Name = playerRecord.Name;
                result.kill = playerRecord.KillCount;
                result.death = playerRecord.DeathCount;
                result.bombInsert = playerRecord.BombInsertCount;
                result.gold = playerRecord.EarnGold;
                result.isMvp = playerRecord.IsMvp;

                Managers.Game.PlayerResultData.Add(result);
            }

            

            if(Managers.Scene.CurrentScene.GetComponent<GameScene>() != null)
            {
                Managers.Scene.CurrentScene.GetComponent<GameScene>().WaitAndShowResult();
            }
        }

        
    }
}
