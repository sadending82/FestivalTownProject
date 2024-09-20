using Google.FlatBuffers;
using PacketTable.PlayerTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Network.PacketProcessor
{
    public class PlayerGroggyRecoveryProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var Data = PlayerGroggyRecovery.GetRootAsPlayerGroggyRecovery(bb);

            int id = Data.Id;
            int staminaRecoveryValue = Data.RecoveryValue;


        }
    }
}
