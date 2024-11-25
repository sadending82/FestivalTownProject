using Google.FlatBuffers;
using NetworkProtocol;
using PacketTable.GameTable;
using PacketTable.LoginTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Network.PacketProcessor
{
    public class VersionCheckResponseProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var Data = VersionCheckResponse.GetRootAsVersionCheckResponse(bb);

            int result = Data.Result;

            if (result != (int)ERROR_CODE.ER_NONE)
            {
                Debug.Log($"Version Check Result Code : {result}");

                var ui = Managers.UI.ShowPopUpUI<UI_Notice>();
                ui.Init();
                ui.NoticeTextChange("버전이 다릅니다.\n 최신 버전으로 업데이트 해주세요.");

                ui.BindGameEndEvent();

            }
        }


    }
}
