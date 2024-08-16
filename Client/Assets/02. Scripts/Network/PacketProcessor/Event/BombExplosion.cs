using Google.FlatBuffers;
using PacketTable.ObjectTable;
using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting.FullSerializer;
using UnityEngine;
using static UnityEditor.PlayerSettings;


namespace Network.PacketProcessor
{
    public class BombExplosionProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var Data = BombExplosion.GetRootAsBombExplosion(bb);
            Vector3 pos = new Vector3(Data.Pos.Value.X, Data.Pos.Value.Y, Data.Pos.Value.Z);
            int bombId = Data.Id;

            GameObject targetBomb = Managers.BombObject.FindBombById(bombId);
            if (targetBomb != null)
            {
                targetBomb.transform.position = pos;
                targetBomb.GetComponent<Bomb>().Boom();
            }
            else
            {
                Debug.Log("BombExplosionProcessor Target Bomb is NULL !!!");
            }
        }

        private FlatBufferBuilder mBuilder;
    }
}
