using Google.FlatBuffers;
using NetworkProtocol;
using PacketTable.PlayerTable;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

namespace Network.PacketProcessor
{
    public class PlayerMoveProcessor : PacketProcessor
    {
        public override void Process(byte[] data, GameObject playerManager)
        {
            // 여기에 처리    
            var bb = new ByteBuffer(data);
            var moveData = PlayerMove.GetRootAsPlayerMove(bb);

            int id = moveData.Id;// 이거는 일단 안씀
            Vector3 pos = new Vector3(moveData.Pos.Value.X, moveData.Pos.Value.Y, moveData.Pos.Value.Z);
            Vector3 dir = new Vector3(moveData.Direction.Value.X, moveData.Direction.Value.Y, moveData.Direction.Value.Z);
            int state = moveData.State;

            PlayerController pController = playerManager.transform.GetChild(id).GetComponent<PlayerController>();

            if (pController != null)
            {
                switch (state)
                {
                    case (int)ePlayerState.PS_RUN:
                        {
                            pController.SetDirection(dir);
                            pController.SetPosition(pos);
                            pController.SetIsMove(true);
                        }
                        break;

                    case (int)ePlayerState.PS_JUMP:
                        {
                            pController.Jump();
                        }
                        break;

                    default:
                        {

                        }
                        break;
                }
            }
        }

        private FlatBufferBuilder mBuilder;
    }
}
