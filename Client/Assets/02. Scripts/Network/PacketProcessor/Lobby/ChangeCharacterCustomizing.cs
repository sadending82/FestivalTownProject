using ExcelDataStructure;
using Google.FlatBuffers;
using NetworkProtocol;
using PacketTable.LobbyTable;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Unity.VisualScripting;
using UnityEngine;

namespace Network.PacketProcessor
{
    public class ChangeCharacterCustomizingProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = ChangeCharacterCustomizing.GetRootAsChangeCharacterCustomizing(bb);

            for (int i = 0; i < Data.CustomizingItemsLength; i++)
            {
                var item = Data.CustomizingItems(i).Value;

                int item_UID = item.ItemUid;
                int item_Code = item.ItemCode;
                int item_Type = item.Type;
                Debug.Log("창");
                GameObject tPlayer = Managers.Player.GetMyPlayer();
                tPlayer.GetComponent<CharacterStatus>().ChangeCustomizing(item_Code);
            }
        }
    }
}
