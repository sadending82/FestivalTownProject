using Google.FlatBuffers;
using PacketTable.GameTable;
using PacketTable.LoginTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


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
                Managers.Data.SetGold(Data.Gold);
                Managers.Data.SetDiamond(Data.Point);
                Managers.Data.SetNickName(Data.Nickname);
                Managers.Scene.LoadScene(Define.Scene.Home);
            }
        }


    }
}
