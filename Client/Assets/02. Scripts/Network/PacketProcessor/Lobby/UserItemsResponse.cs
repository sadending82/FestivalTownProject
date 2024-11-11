using ExcelDataStructure;
using Google.FlatBuffers;
using PacketTable.LobbyTable;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Unity.VisualScripting;
using UnityEngine;

namespace Network.PacketProcessor
{
    public class UserItemsResponseProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = UserItemsResponse.GetRootAsUserItemsResponse(bb);

            if (Data.Result == 1)
            {
                for (int i = 0; i < Data.ItemsLength; ++i)
                {
                    var item_info = Data.Items(i).Value;

                    int item_uid = item_info.ItemUid;
                    int item_code = item_info.ItemCode;
                    int count = item_info.Count;
                    int item_type = item_info.ItemType;
                }
            }
            else
            {
                
            }
        }


    }
}
