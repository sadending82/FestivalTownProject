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

        }


    }
}
