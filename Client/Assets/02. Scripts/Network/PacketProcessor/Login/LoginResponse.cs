using Google.FlatBuffers;
using PacketTable.UtilitiesTable;
using PacketTable.LoginTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering.PostProcessing;


namespace Network.PacketProcessor
{
    public class LoginResponseProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var Data = LoginResponse.GetRootAsLoginResponse(bb);

            int resultCode = Data.ResultCode;

            Debug.Log($"Login Result : {resultCode}");

            if (resultCode == 1)
            {
                var userInfo = Data.UserInfo.Value;
                Debug.Log($"gold : {Data.Gold}, dia : {Data.Dia}");
                Managers.Data.SetGold(Data.Gold);
                Managers.Data.SetDiamond(Data.Dia);
                Managers.Data.SetNickName(userInfo.Nickname);
                Managers.Scene.LoadScene(Define.Scene.Home);
            }
        }


    }
}
