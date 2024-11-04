using Google.FlatBuffers;
using PacketTable.GameTable;
using PacketTable.LoginTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Network.PacketProcessor
{
    public class VersionCheckProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            //var Data = VersionCheck.GetRootAsSignUpResponse(bb);

          
        }


    }
}
