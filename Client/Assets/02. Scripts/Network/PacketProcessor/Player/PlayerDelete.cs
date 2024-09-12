using Google.FlatBuffers;
using PacketTable.PlayerTable;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;
using static UnityEditor.Experimental.GraphView.GraphView;
using static UnityEngine.GraphicsBuffer;

namespace Network.PacketProcessor
{
    public class PlayerDeleteProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = PlayerDelete.GetRootAsPlayerDelete(bb);
            var id = Data.Id;

            Debug.Log("Delete player - " + id);
            Managers.Player.GetPlayers().transform.GetChild(id).GetComponent<PlayerController>().gameObject.SetActive(false);
        }

        private FlatBufferBuilder mBuilder;
    }
}
