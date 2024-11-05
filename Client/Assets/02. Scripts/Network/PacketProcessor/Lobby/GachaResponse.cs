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

            var popup = Managers.UI.ShowPopUpUI<UI_Notice>();
            popup.Init();

            popup.NoticeTextChange($"나온 아이템은 Type : {Data.AcquiredItemType}, {Data.AcquiredItemAmount}개 입니다.\n" +
                $"사용한 재화는 Type : {Data.SpentResourceType}를 {Data.SpentResourceAmount}개 사용하였습니다.");
            popup.BindPopupCloseEvent();
        }


    }
}
