using Google.FlatBuffers;
using NetworkProtocol;
using PacketTable.GameTable;
using PacketTable.LobbyTable;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;


namespace Network.PacketProcessor
{
    public class GameMatchingResponseProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = GameMatchingResponse.GetRootAsGameMatchingResponse(bb);
            int mapIndex = Data.MapIndex;
            int mapTheme = Data.MapTheme;
            int gameMode = Data.Gamemode;

            Debug.Log("Game Maching Complete");
            Managers.Game.Init();
            Managers.Game.RoomID = Data.Roomid;
            Managers.Game.inGameID = Data.Ingameid;
            Managers.Game.isHost = Data.IsHost;
            Managers.Game.mapIndex = Data.MapIndex;
            Managers.Game.mapTheme = Data.MapTheme;
            Managers.Game.gameMode = (eGameMode)Data.Gamemode;
            Managers.Game.currentTeamID = Data.Team;
            Managers.Game.GameRemainTime = Data.GameTime; // Seconds

            if (Managers.Data.MapThemeDataDict.TryGetValue(mapIndex, out var themeData))
            {
                Managers.Game.ModeTitle = themeData.Mode_Name;
                Managers.Game.ModeDescription = themeData.Mode_Description;
                List<int> TipTypeindices = Managers.Data.GetTipTypeIndices(mapIndex);
                Managers.Game.TipIndices = Managers.Data.GetTipIndices(TipTypeindices);           
            }

            Managers.Scene.LoadScene(Define.Scene.Loading);

            Debug.Log("MapIndex: " + mapIndex + " MapTheme: " + mapTheme);


            // 플레이어 정보
            for (int i = 0; i < Data.PlayersLength; ++i)
            {
                var player = Data.Players(i);
                int id = player.Value.Id;
                int team = player.Value.Team;
                int characterType = player.Value.CharacterType;
                string nickname = player.Value.Nickname;

                var position = player.Value.Pos.Value;

                var characterCustomizing = player.Value.CharacterCustomizing.Value;

                for (int j = 0; j < characterCustomizing.CustomizingItemsLength; j++)
                {
                    var item = characterCustomizing.CustomizingItems(j).Value;

                    int item_UID = item.ItemUid;
                    int item_Code = item.ItemCode;
                    int item_Type = item.Type;
                    Managers.Game.SetCharacterCustomizingById(id, item_Code);
                }

                Debug.Log($"AddPlayer info ID : {id}, TEAM : {team}, POS: ({position.X}, {position.Y}, {position.Z}), NICKNAME : {nickname}");

                Vector3 pos = new Vector3(position.X, position.Y, position.Z);
                Managers.Player.GetComponent<PlayerManager>().AddPlayer(id, pos, team, nickname);
                Managers.Game.PlayerTeamData.Add(id, team);

            }
        }
    }
}
