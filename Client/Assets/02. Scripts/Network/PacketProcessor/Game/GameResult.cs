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

            List<Define.PlayerResult> team1List = new();
            List<Define.PlayerResult> team2List = new();

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
                result.point = playerRecord.Point;

                if (Managers.Game.gameMode == NetworkProtocol.eGameMode.FITH_Indiv_Battle_3)
                {
                    Managers.Game.PlayerResultData.Add(result);
                }
                else
                {
                    Debug.Log($"{result.Id} 플레이어 데이터. {Managers.Game.PlayerTeamData[result.Id]}");
                    if (Managers.Game.PlayerTeamData[result.Id] == 0)
                    {
                        team1List.Add(result);
                    }
                    else
                    {
                        team2List.Add(result);
                    }
                }
            }


            if (Managers.Game.gameMode == NetworkProtocol.eGameMode.FITH_Indiv_Battle_3)
            {
                Managers.Game.PlayerResultData.Sort((Define.PlayerResult a, Define.PlayerResult b) => { return b.point.CompareTo(a.point); });
            }
            else
            {
                team1List.Sort((Define.PlayerResult a, Define.PlayerResult b) => { return b.point.CompareTo(a.point); });
                team2List.Sort((Define.PlayerResult a, Define.PlayerResult b) => { return b.point.CompareTo(a.point); });
                
                if(winningTeam == 0)
                {
                    for(int i = 0; i < team1List.Count; i++)
                    {
                        Managers.Game.PlayerResultData.Add(team1List[i]);
                    }

                    for (int i = 0; i < team2List.Count; i++)
                    {
                        Managers.Game.PlayerResultData.Add(team2List[i]);
                    }
                }
                else
                {
                    for (int i = 0; i < team2List.Count; i++)
                    {
                        Managers.Game.PlayerResultData.Add(team2List[i]);
                    }

                    for (int i = 0; i < team1List.Count; i++)
                    {
                        Managers.Game.PlayerResultData.Add(team1List[i]);
                    }
                }
            }


            if(Managers.Scene.CurrentScene.GetComponent<GameScene>() != null)
            {
                Managers.Scene.CurrentScene.GetComponent<GameScene>().WaitAndShowResult();
            }
        }

        
    }
}
