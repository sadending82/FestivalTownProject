using ExcelDataStructure;
using Google.FlatBuffers;
using NetworkProtocol;
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

            var gachaCutScene = Managers.UI.GetCurrentSceneUI();

            if (Data.Result == (int)ERROR_CODE.ER_NONE)
            {
                // 가챠 결과 아이템
                var resultItemData = Managers.Data.GetItemData(Data.ResultItemCode);
                // 실제 지급 아이템
                var acquiredItemData = Managers.Data.GetItemData(Data.AcquiredItemCode);

                gachaCutScene.GetComponent<UI_GachaCutScene>().SetResultItemCode(Data.ResultItemCode);
                gachaCutScene.GetComponent<UI_GachaCutScene>().SetAcquiredItemCode(Data.AcquiredItemCode);
                gachaCutScene.GetComponent<UI_GachaCutScene>().SetAcquiredItemAmount(Data.AcquiredItemAmount);

                Debug.Log($"Gacha ItemCode : {Data.ResultItemCode}, Amount : {Data.ResultItemAmount}, SpentType : {Data.SpentResourceCode}," +
                    $"SpentAmount : {Data.SpentResourceAmount}, GachaGrade : {resultItemData.Item_Grade}");

                if (resultItemData == null) { return; }

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
        }


    }
}
