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
                // 가챠 결과 아이템
                var resultItemData = Managers.Data.GetItemData(Data.ResultItemCode);
                // 실제 지급 아이템
                var acquiredItemData = Managers.Data.GetItemData(Data.AcquiredItemCode);

                Debug.Log($"Gacha ItemType : {Data.ResultItemAmount}, Amount : {Data.ResultItemAmount}, SpentType : {Data.SpentResourceCode}," +
                    $"SpentAmount : {Data.SpentResourceAmount}");

                if (resultItemData == null) { return; }

                ItemEntity ie = (ItemEntity)resultItemData;

                if (Data.ResultItemCode == Data.AcquiredItemCode)
                {
                    popup.NoticeTextChange($"나온 아이템은 {resultItemData.Name}, {Data.AcquiredItemAmount}개 입니다.");
                }
                else
                {
                    popup.NoticeTextChange($"나온 아이템은 {resultItemData.Name}, {Data.ResultItemAmount}개 입니다.\n"
                        + $"이미 소유한 아이템이므로 {acquiredItemData.Name}, {Data.AcquiredItemAmount}개로 전환되었습니다.");
                }

                popup.BindPopupCloseEvent();

                switch (Managers.Data.GetResourceIndexType(Data.AcquiredItemCode))
                {
                    case DataManager.ResourceIndexType.Gold:
                        int gold = Managers.Data.GetGold();
                        gold += Data.AcquiredItemAmount;
                        Managers.Data.SetGold(gold);
                        break;
                    case DataManager.ResourceIndexType.Diamond:
                        int dia = Managers.Data.GetDiamond();
                        dia += Data.AcquiredItemAmount;
                        Managers.Data.SetDiamond(dia);
                        break;
                    case DataManager.ResourceIndexType.Mileage:
                        int mileage = Managers.Data.GetMileage();
                        mileage += Data.AcquiredItemAmount;
                        Managers.Data.SetMileage(mileage);
                        break;
                    case DataManager.ResourceIndexType.TicketBase:
                        int ticketBase = Managers.Data.GetTicketBase();
                        ticketBase += Data.AcquiredItemAmount;
                        Managers.Data.SetTicketBase(ticketBase);
                        break;
                    case DataManager.ResourceIndexType.TicketRare:
                        int ticketRare = Managers.Data.GetTicketRare();
                        ticketRare += Data.AcquiredItemAmount;
                        Managers.Data.SetTicketRare(ticketRare);
                        break;
                    default:
                        break;
                }

                switch (Managers.Data.GetResourceIndexType(Data.SpentResourceCode))
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
