using ExcelDataStructure;
using Google.FlatBuffers;
using PacketTable.LobbyTable;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

namespace Network.PacketProcessor
{
    public class GachaResponseProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = GachaResponse.GetRootAsGachaResponse(bb);



            var itemData = Managers.Data.GetData(Data.AcquiredItemType);

            Debug.Log($"Gacha ItemType : {Data.AcquiredItemType}, Amount : {Data.AcquiredItemAmount}, SpentType : {Data.SpentResourceType}," +
                $"SpentAmount : {Data.SpentResourceAmount}");

            if (itemData == null) { return; }

            var popup = Managers.UI.ShowPopUpUI<UI_Notice>();
            popup.Init();

            ItemEntity ie = (ItemEntity)itemData;

            popup.NoticeTextChange($"나온 아이템은 Type : {itemData.Name}, {Data.AcquiredItemAmount}개 입니다.");

            popup.BindPopupCloseEvent();
        }


    }
}
