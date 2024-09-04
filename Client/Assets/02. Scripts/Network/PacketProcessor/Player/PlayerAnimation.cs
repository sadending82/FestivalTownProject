using Google.FlatBuffers;
using PacketTable.PlayerTable;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Unity.VisualScripting.FullSerializer;
using UnityEngine;
using ClientProtocol;

namespace Network.PacketProcessor
{
    public class PlayerAnimationProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);
            var Data = PlayerAnimation.GetRootAsPlayerAnimation(bb);

            int id = Data.Id;
            Vector3 pos = new Vector3(Data.Pos.Value.X, Data.Pos.Value.Y, Data.Pos.Value.Z);
            Vector3 dir = new Vector3(Data.Direction.Value.X, Data.Direction.Value.Y, Data.Direction.Value.Z);
            int aniState = Data.Animation;

            PlayerController pController = Managers.Player.GetPlayers().transform.GetChild(id).GetComponent<PlayerController>();
            pController.SetDirection(dir);
            pController.SetPosition(pos);

            CharacterStatus pState = Managers.Player.GetPlayers().transform.GetChild(id).GetComponent<CharacterStatus>();
            pState.s_SetUpperBodyAnimationState((UpperBodyAnimationState)aniState);
        }

        private FlatBufferBuilder mBuilder;
    }
}
