using Google.FlatBuffers;
using PacketTable.PlayerTable;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;
using NetworkProtocol;

namespace Network.PacketProcessor
{
    public class PlayerStopProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            // 여기에 처리    
            var bb = new ByteBuffer(data);
            var stopData = PlayerStop.GetRootAsPlayerStop(bb);

            int id = stopData.Id; // 이거는 일단 안씀
            Vector3 pos = new Vector3(stopData.Pos.Value.X, stopData.Pos.Value.Y, stopData.Pos.Value.Z);
            Vector3 dir = new Vector3(stopData.Direction.Value.X, stopData.Direction.Value.Y, stopData.Direction.Value.Z);
            int state = stopData.State;

            if (Managers.Player == null) { return; }

            PlayerController pController = Managers.Player.GetPlayers().transform.GetChild(id).GetComponent<PlayerController>();

            pController.SetPosition(pos);
            pController.SetDirection(dir);

            if (pController != null)
            {
                switch (state)
                {
                    case (int)ePlayerMoveState.PS_MOVESTOP:
                        {
                            pController.SetIsMove(false);
                            pController.s_SetAnimation(ePlayerMoveState.PS_MOVESTOP);
                        }
                        break;

                    case (int)ePlayerMoveState.PS_JUMPSTOP:
                        {
                            pController.s_SetAnimation(ePlayerMoveState.PS_JUMPSTOP);
                        }
                        break;
                    default:
                        {
                            Debug.Log("ERROR!! : PlayerStop.cs, Wrong MoveState !!!");
                        }
                        break;
                }
            }
        }

        private FlatBufferBuilder mBuilder;
    }
}
