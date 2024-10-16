﻿using Google.FlatBuffers;
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
                Managers.UI.ShowPopUpUI<UI_SignInSuccess>();
            }
            else
            {
                if (Camera.main.gameObject.GetComponent<SuperBlur.SuperBlur>() != null)
                {
                    Camera.main.gameObject.GetComponent<SuperBlur.SuperBlur>().enabled = false;
                }

                Managers.UI.ClosePopUpUI();
            }
        }


    }
}
