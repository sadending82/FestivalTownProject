using ExcelDataStructure;
using Google.FlatBuffers;
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
    public class UserItemsResponseProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = UserItemsResponse.GetRootAsUserItemsResponse(bb);

            Managers.Data.InventoryDataList.Clear();

            if (Data.Result == 1)
            {
                for (int i = 0; i < Data.ItemsLength; ++i)
                {
                    var item_info = Data.Items(i).Value;

                    Define.ItemData item = new Define.ItemData();
                    item.ItemUid = item_info.ItemUid;
                    item.ItemCode = item_info.ItemCode;
                    item.Count = item_info.Count;
                    item.Type = item_info.ItemType;

                    Managers.Data.InventoryDataList.Add(item.ItemCode, item);
                }

                Managers.Data.SetInventoryDataRecved(true);
            }
            else
            {
                var ui = Managers.UI.ShowPopUpUI<UI_Notice>();
                ui.Init();
                ui.NoticeTextChange("아이템 데이터를 받아올 수 없습니다.");
                ui.BindPopupCloseEvent();
            }
        }


    }
}
