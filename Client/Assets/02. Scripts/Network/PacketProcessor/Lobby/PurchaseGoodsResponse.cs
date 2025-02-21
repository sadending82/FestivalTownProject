using Google.FlatBuffers;
using NetworkProtocol;
using PacketTable.ShopTable;

using System.Collections.Generic;

using UnityEngine;

namespace Network.PacketProcessor
{
    public class PurchaseGoodsResponseProcessor : PacketProcessor
    {
        public override void Process(PacketManager packetmanager, byte[] data)
        {
            var bb = new ByteBuffer(data);

            var Data = PurchaseGoodsResponse.GetRootAsPurchaseGoodsResponse(bb);

            int result = Data.Result;

            if (result == 0)
            {

            }
            else if (result == 1)
            {

                int item_index = Data.ItemIndex;
                int item_amount = Data.ItemAmount;

                Debug.Log($"Recv PurchaseGoodsPacket item index : {item_index}");

                var shopUI = Managers.UI.GetCurrentSceneUI().GetComponent<UI_Shop>();
                if (shopUI != null)
                {
                    shopUI.GetShopItem(item_index).PurchaseStuff();
                }

                int currency_index = Data.CurrencyIndex;
                int currency_amount = Data.CurrencyAmount;
            }
        }
    }
}
