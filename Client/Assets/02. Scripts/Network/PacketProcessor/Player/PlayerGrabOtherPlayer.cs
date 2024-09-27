using Google.FlatBuffers;
using PacketTable.PlayerTable;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Unity.VisualScripting.FullSerializer;
using UnityEngine;

namespace Network.PacketProcessor
{
    public class PlayerGrabOtherPlayerProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = PlayerGrabOtherPlayer.GetRootAsPlayerGrabOtherPlayer(bb);

            int id = Data.Id;
            Vector3 position = new Vector3 (Data.Pos.Value.X, Data.Pos.Value.Y, Data.Pos.Value.Z);
            Vector3 direction = new Vector3(Data.Direction.Value.X, Data.Direction.Value.Y, Data.Direction.Value.Z);
            Vector3 handPos = new Vector3(Data.HandPos.Value.X, Data.HandPos.Value.Y, Data.HandPos.Value.Z);
            bool isLeftHand = Data.IsLeftHand;

            int targetId = Data.TargetId;
            Vector3 targetHeadPos = new Vector3(Data.TargetHeadPos.Value.X, Data.TargetHeadPos.Value.Y, Data.TargetHeadPos.Value.Z);

            PlayerController playerController = Managers.Player.FindPlayerById(id).GetComponent<PlayerController>();
            if (playerController != null)
            {
                playerController.SetPosition(position);
                playerController.SetDirection(direction);
                playerController.s_GrapPlayer(targetId, isLeftHand, handPos, targetHeadPos);
            }
            else
            {
                Debug.Log("Error!!!: PlayerGrabOtherPlayer Can't Find Player !!!");
            }
        }
    }
}
