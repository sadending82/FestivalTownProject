using Google.FlatBuffers;
using PacketTable.PlayerTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using static UnityEngine.GraphicsBuffer;


namespace Network.PacketProcessor
{
    public class PlayerGroggyProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var Data = PlayerGroggy.GetRootAsPlayerGroggy(bb);

            int id = Data.Id;
            int hp = Data.Hp;

            CharacterStatus pState = Managers.Player.FindPlayerById(id).GetComponent<CharacterStatus>();
            pState.GroggyOn();
            pState.SetHp(hp);
        }        
    }
}
