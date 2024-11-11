using Google.FlatBuffers;
using NetworkProtocol;
using PacketTable.ObjectTable;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Network.PacketProcessor
{
    public class BlockDropProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var Data = BlockDrop.GetRootAsBlockDrop(bb);

            if (Managers.Game.isInGame)
            {
                for (int i = 0; i < Data.PosLength; ++i)
                {

                    int posX = Data.Pos(i).Value.X;
                    int posY = Data.Pos(i).Value.Y;
                    int type = Data.Type(i);

                    Managers.CubeObject.SpawnCube(posX, posY, (eBlockType)type);
                }
            }
        }

        
    }
}
