using Google.FlatBuffers;
using PacketTable.GameTable;
using PacketTable.LoginTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Network.PacketProcessor
{
    public class SignUpResponseProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var Data = SignUpResponse.GetRootAsSignUpResponse(bb);

            int resultCode = Data.ResultCode;

            Debug.Log($"SignUp Result Code : {resultCode}");



            if (resultCode == 1)
            {
                var ui = Managers.UI.ShowPopUpUI<UI_SignInSuccess>();
                ui.Init();
                ui.TextChange("회원가입에 성공했습니다.");
            }
            else
            {
                var ui = Managers.UI.ShowPopUpUI<UI_Notice>();
                ui.Init();
                ui.NoticeTextChange("회원가입에 실패했습니다.");

                ui.BindPopupCloseEvent();
            }
        }


    }
}
