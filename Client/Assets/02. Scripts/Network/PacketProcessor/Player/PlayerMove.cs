using Google.FlatBuffers;
using NetworkProtocol;
using PacketTable.PlayerTable;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

namespace Network.PacketProcessor
{
    public class PlayerMoveProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var moveData = PlayerMove.GetRootAsPlayerMove(bb);

            int id = moveData.Id;
            Vector3 pos = new Vector3(moveData.Pos.Value.X, moveData.Pos.Value.Y, moveData.Pos.Value.Z);
            Vector3 dir = new Vector3(moveData.Direction.Value.X, moveData.Direction.Value.Y, moveData.Direction.Value.Z);
            int state = moveData.State;

            PlayerController pController = Managers.Player.GetPlayers().transform.GetChild(id).GetComponent<PlayerController>();

            pController.SetPosition(pos);
            pController.SetDirection(dir);
            if (pController != null)
            {
                switch (state)
                {
                    case (int)ePlayerMoveState.PS_WALK:
                        {
                            pController.SetIsMove(true);
                            pController.s_SetAnimation(ePlayerMoveState.PS_WALK);
                        }
                        break;
                    case (int)ePlayerMoveState.PS_RUN:
                        {
                            pController.SetIsMove(true);
                            pController.s_SetAnimation(ePlayerMoveState.PS_RUN);
                        }
                        break;

                    case (int)ePlayerMoveState.PS_JUMP:
                        {
                            pController.SetIsMove(true);
                            pController.SetIsJump(true);
                        }
                        break;

                    case (int)ePlayerMoveState.PS_DASH:
                        {
                            pController.SetIsMove(true);
                            pController.SetIsDash(true);
                        }
                        break;
                    default:
                        {
                            UnityEngine.Debug.Log("ERROR!! : PlayerMove.cs, Wrong MoveState !!!");
                        }
                        break;
                }
            }
        }

        
    }
}
