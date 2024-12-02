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
using NetworkProtocol;

namespace Network.PacketProcessor
{
    public class CurrencyAmountResponseProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = CurrencyAmountResponse.GetRootAsCurrencyAmountResponse(bb);

            if (Data.Result == (int)ERROR_CODE.ER_NONE)
            {
                for (int i = 0; i < Data.CurrencyAmountLength; ++i)
                {
                    int Code = Data.CurrencyCode(i);
                    int amount = Data.CurrencyAmount(i);

                    switch(Managers.Data.GetResourceIndexType(Code))
                    {
                        case DataManager.ResourceIndexType.Gold:
                            Managers.Data.SetGold(amount);
                            break;
                        case DataManager.ResourceIndexType.Diamond:
                            Managers.Data.SetDiamond(amount);
                            break;
                        case DataManager.ResourceIndexType.Mileage:
                            Managers.Data.SetMileage(amount);
                            break;
                        case DataManager.ResourceIndexType.TicketBase:
                            Managers.Data.SetTicketBase(amount);
                            break;
                        case DataManager.ResourceIndexType.TicketRare:
                            Managers.Data.SetTicketRare(amount);
                            break;
                        default:
                            Debug.Log("재화 데이터 잘못 받았음");
                            break;
                    }
                }
            }
            else if (Data.Result == (int)ERROR_CODE.ER_DB_ERROR)
            {
                var popup = Managers.UI.ShowPopUpUI<UI_Notice>();
                popup.Init();

                popup.NoticeTextChange($"재화를 불러올 수 없습니다.");

                popup.BindPopupCloseEvent();
            }
        }


    }
}
