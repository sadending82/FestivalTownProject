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

                int goods_index = Data.GoodsIndex;

                Debug.Log($"Recv PurchaseGoodsPacket goods index : {goods_index}");

                // 403001 == 패스
                if(goods_index == 403001)
                {
                    var passUI = Managers.UI.GetCurrentSceneUI().GetComponent<UI_Pass>();
                    if (passUI == null) return;

                    passUI.PassActivate();

                    return;
                }

                var shopUI = Managers.UI.GetCurrentSceneUI().GetComponent<UI_Shop>();
                if (shopUI != null)
                {
                    // 마일리지 샵 리소스에 '보유중'과 '구매 완료'가 나누어져 있어 두개가 서로 다른 상태이므로
                    // 구매한 경우, 구매 완료로 따로 설정해 주기 위한 부분.

                    // 현재 ShopList 인덱스로 설정되어 있어 혹시라도 실제 item의 인덱스라면 에러가 발생함.
                    // ShopList의 인덱스라면 문제 없을 것
                    var shopItemUI = shopUI.GetShopItem(goods_index);
                    if (shopItemUI != null)
                    {
                        shopItemUI.PurchaseStuff();
                    }
                    else
                    {
                        Debug.Log($"해당 인덱스를 가지는 상품 항목이 없습니다.");
                        return;
                    }
                }

                int currency_index = Data.CurrencyIndex;
                int currency_amount = Data.CurrencyAmount;

                var type = Managers.Data.GetResourceIndexType(currency_index);
                // Managers.Data.GetResourceByType(type); -> 데이터 가져오기
                Managers.Data.SetResourceByType(type, currency_amount);
            }
        }
    }
}
