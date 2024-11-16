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

            // 1. 그냥 때려 박음
            int skin_itemUID = Data.CustomizingItemsByKey((int)CustomizingItemType.CI_SKIN).Value.ItemUid;
            int skin_itemCode = Data.CustomizingItemsByKey((int)CustomizingItemType.CI_SKIN).Value.ItemCode;

            int face_itemUID = Data.CustomizingItemsByKey((int)CustomizingItemType.CI_FACE).Value.ItemUid;
            int face_itemCode = Data.CustomizingItemsByKey((int)CustomizingItemType.CI_FACE).Value.ItemCode;

            int head_itemUID = Data.CustomizingItemsByKey((int)CustomizingItemType.CI_HEAD).Value.ItemUid;
            int head_itemCode = Data.CustomizingItemsByKey((int)CustomizingItemType.CI_HEAD).Value.ItemCode;

            int back_itemUID = Data.CustomizingItemsByKey((int)CustomizingItemType.CI_BACK).Value.ItemUid;
            int back_itemCode = Data.CustomizingItemsByKey((int)CustomizingItemType.CI_BACK).Value.ItemCode;


            // 2. for문돌림 알아서 골라서 쓰셈
            for (int i = 0; i < Data.CustomizingItemsLength; i++)
            {
                var item = Data.CustomizingItems(i).Value;

                int item_UID = item.ItemUid;
                int item_Code = item.ItemCode;
                int item_Type = item.Type;

            }
        }


    }
}
