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
    public class CurrencyAmountResponseProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = CurrencyAmountResponse.GetRootAsCurrencyAmountResponse(bb);

            if (Data.Result == 1)
            {
                for (int i = 0; i < Data.CurrencyAmountLength; ++i)
                {
                    int type = Data.CurrencyType(i);
                    int amount = Data.CurrencyAmount(i);

                    // 여기에 추가 작업 필요
                }
            }
            else
            {
                var popup = Managers.UI.ShowPopUpUI<UI_Notice>();
                popup.Init();

                popup.NoticeTextChange($"재화를 불러올 수 없습니다.");

                popup.BindPopupCloseEvent();
            }
        }


    }
}
