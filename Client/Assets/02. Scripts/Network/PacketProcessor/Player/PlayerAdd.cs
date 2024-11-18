using Google.FlatBuffers;
using PacketTable.PlayerTable;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;
using static UnityEngine.GraphicsBuffer;

namespace Network.PacketProcessor
{
    public class PlayerAddProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = PlayerAdd.GetRootAsPlayerAdd(bb);

            for (int i = 0; i < Data.PlayersLength; ++i)
            {
                var player = Data.Players(i);
                int id = player.Value.Id;
                int team = player.Value.Team;
                int characterType = player.Value.CharacterType;
                string nickname = player.Value.Nickname;

                var characterCustomizing = player.Value.CharacterCustomizing.Value;

                for (int j = 0; j < characterCustomizing.CustomizingItemsLength; j++)
                {
                    var item = characterCustomizing.CustomizingItems(j).Value;

                    int item_UID = item.ItemUid;
                    int item_Code = item.ItemCode;
                    int item_Type = item.Type;
                    Debug.Log($"AddPlayer info ITEM UID : {item_UID}, ITEM CODE : {item_Code}, ITEM TYPE : {item_Type}");
                    Managers.Game.SetCharacterCustomizingById(id, item_Code);
                }

                Vector3 pos = new Vector3(player.Value.Pos.Value.X, player.Value.Pos.Value.Y, player.Value.Pos.Value.Z);
                Managers.Player.GetComponent<PlayerManager>().AddPlayer(id, pos, team);
            }
        }
    }
}
