using Google.FlatBuffers;
using PacketTable.PlayerTable;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Unity.VisualScripting.FullSerializer;
using UnityEditor;
using UnityEngine;

namespace Network.PacketProcessor
{
    public class PlayerThrowOtherPlayerProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = PlayerThrowOtherPlayer.GetRootAsPlayerThrowOtherPlayer(bb);

            int id = Data.Id;
            Vector3 position = new Vector3(Data.Pos.Value.X, Data.Pos.Value.Y, Data.Pos.Value.Z);
            Vector3 direction = new Vector3(Data.Direction.Value.X, Data.Direction.Value.Y, Data.Direction.Value.Z);

            int targetId = Data.TargetId;
            Vector3 targetPos = new Vector3(Data.TargetPos.Value.X, Data.TargetPos.Value.Y, Data.TargetPos.Value.Z);
            Vector3 targetDirection = new Vector3(Data.TargetDirection.Value.X, Data.TargetDirection.Value.Y, Data.TargetDirection.Value.Z);

            PlayerController playerController = Managers.Player.FindPlayerById(id).GetComponent<PlayerController>();
            if (playerController != null)
            {
                playerController.SetPosition(position);
                playerController.SetDirection(direction);
                playerController.s_ThrowPlayer();
            }
            else
            {
                Debug.Log("Error!!!: PlayerThrowOtherPlayer Can't Find Player !!!");
            }

            PlayerController targetPlayerController = Managers.Player.FindPlayerById(targetId).GetComponent<PlayerController>();
            CharacterStatus targetPlayerState = Managers.Player.FindPlayerById(targetId).GetComponent<CharacterStatus>();
            targetPlayerState.SetIsGrapped(false);
            if (targetPlayerController != null)
            {
                targetPlayerController.SetPosition(targetPos);
                targetPlayerController.SetDirection(targetDirection);
            }
            else
            {
                Debug.Log("Error!!!: PlayerThrowOtherPlayer Can't Find Target Player !!!");
            }
        }
    }
}
