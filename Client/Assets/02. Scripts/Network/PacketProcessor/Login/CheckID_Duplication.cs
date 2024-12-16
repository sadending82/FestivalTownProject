using Google.FlatBuffers;
using NetworkProtocol;
using PacketTable.GameTable;
using PacketTable.LoginTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Network.PacketProcessor
{
    public class CheckID_DuplicationProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var Data = CheckID_Duplication.GetRootAsCheckID_Duplication(bb);

            int result = Data.Result;

            Debug.Log($"CheckID_Duplication Result Code : {result}");

            // 중복 없는 경우
            if (result == 0)
            {
                var ui = Managers.UI.ShowPopUpUI<UI_Notice>();
                ui.Init();
                ui.NoticeTextChange("사용 가능한 ID 입니다.");

                ui.BindPopupCloseEvent();

            }
            // 중복 있는 경우
            else
            {
                var ui = Managers.UI.ShowPopUpUI<UI_Notice>();
                ui.Init();
                ui.NoticeTextChange("이미 존재하는 ID 입니다.");

                ui.BindPopupCloseEvent();
            }
        }


    }
}
