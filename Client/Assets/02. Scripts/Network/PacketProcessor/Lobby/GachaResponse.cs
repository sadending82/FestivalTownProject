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

            if (Data.Result == 1)
            {
                var itemData = Managers.Data.GetItemData(Data.AcquiredItemType);

                Debug.Log($"Gacha ItemType : {Data.AcquiredItemType}, Amount : {Data.AcquiredItemAmount}, SpentType : {Data.SpentResourceType}," +
                    $"SpentAmount : {Data.SpentResourceAmount}");

                if (itemData == null) { return; }

                ItemEntity ie = (ItemEntity)itemData;

                popup.NoticeTextChange($"나온 아이템은 {itemData.Name}, {Data.AcquiredItemAmount}개 입니다.");

                popup.BindPopupCloseEvent();

                switch (Managers.Data.GetResourceIndexType(Data.SpentResourceType))
                {
                    case DataManager.ResourceIndexType.Gold:
                        int gold = Managers.Data.GetGold();
                        gold -= Data.SpentResourceAmount;
                        Managers.Data.SetGold(gold);
                        break;
                    case DataManager.ResourceIndexType.Diamond:
                        int dia = Managers.Data.GetDiamond();
                        dia -= Data.SpentResourceAmount;
                        Managers.Data.SetDiamond(dia);
                        break;
                    case DataManager.ResourceIndexType.Mileage:
                        int mileage = Managers.Data.GetMileage();
                        mileage -= Data.SpentResourceAmount;
                        Managers.Data.SetMileage(mileage);
                        break;
                    case DataManager.ResourceIndexType.TicketBase:
                        int ticketBase = Managers.Data.GetTicketBase();
                        ticketBase -= Data.SpentResourceAmount;
                        Managers.Data.SetTicketBase(ticketBase);
                        break;
                    case DataManager.ResourceIndexType.TicketRare:
                        int ticketRare = Managers.Data.GetTicketRare();
                        ticketRare -= Data.SpentResourceAmount;
                        Managers.Data.SetTicketRare(ticketRare);
                        break;
                    default:
                        Debug.Log("가챠 결과 데이터 잘못 받았음");
                        break;
                }

            }
            else
            {
                popup.NoticeTextChange($"재화가 없습니다.");

                popup.BindPopupCloseEvent();
            }
        }


    }
}
