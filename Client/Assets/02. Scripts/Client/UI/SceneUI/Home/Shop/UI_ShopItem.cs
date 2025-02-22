using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class UI_ShopItem : UI_Base
{
    enum GameObjects
    {
        ItemFrame,
        ItemImage,
        ItemName,
        ItemCurrency,
        ItemAlreadyOwned,
        ItemPurchased,
    }

    int itemIdx = -1;
    int shopListIdx = -1;
    bool isAlreadyOwned = false;
    bool isPurchased = false;
    bool isInitialzied = false;

    void Start()
    {
        if (!isInitialzied) Init();
    }

    public override void Init()
    {
        transform.localScale = Vector3.one;

        Bind<GameObject>(typeof(GameObjects));

        isInitialzied = true;
    }

    public void SetItemData(int itemIdx, int shopListIdx, int CurrencyType, int CurrencyAmount, bool isAlreadyOwned, bool isPurchased)
    {
        Managers.Data.ItemDict.TryGetValue(itemIdx, out var itemData);
        if (itemData != null) {
            this.itemIdx = itemIdx;
            this.shopListIdx = shopListIdx;
            Get<GameObject>((int)GameObjects.ItemName).GetComponent<TMP_Text>().text = itemData.Name;
            Get<GameObject>((int)GameObjects.ItemFrame).GetComponent<Image>().sprite = Util.GetItemBGTexture((Define.ItemGrade)itemData.Item_Grade);
            Get<GameObject>((int)GameObjects.ItemImage).GetComponent<Image>().sprite = Managers.Resource.LoadSprite(itemData.File_Name);
            Get<GameObject>((int)GameObjects.ItemImage).GetComponent<Image>().preserveAspect = true;
            Get<GameObject>((int)GameObjects.ItemCurrency).transform.GetChild(0).GetComponent<Image>(); // 따로 이미지 설정해 줘야 하면 설정하기.
            Get<GameObject>((int)GameObjects.ItemCurrency).transform.GetChild(1).GetComponent<TMP_Text>().text = CurrencyAmount.ToString();
            
            
            this.isAlreadyOwned = isAlreadyOwned;
            this.isPurchased = isPurchased;
            SetItemState();

            gameObject.BindEvent((PointerEventData) =>
            {
                if(!isAlreadyOwned && !isPurchased)
                {
                    Debug.Log($"Send PurchaseGoodsPacket item index : {shopListIdx}");
                    Managers.Network.GetPacketManager().SendPurchaseGoodsRequestPacket(this.shopListIdx);
                }
            });
        }
    }

    /// <summary>
    /// 아이템이 보유중인지, 아이템을 이미 구매했는지 확인하고, 해당하는 상태로 변경하는 함수
    /// </summary>
    public void SetItemState()
    {
        AllStateStuffOff();
        if(isAlreadyOwned) Get<GameObject>((int)GameObjects.ItemAlreadyOwned).SetActive(true);
        if(isPurchased) Get<GameObject>((int)GameObjects.ItemPurchased).SetActive(true);
    }

    public void PurchaseStuff()
    {
        isAlreadyOwned = true;
        isPurchased = true;
        SetItemState();
    }

    public void AllStateStuffOff()
    {
        Get<GameObject>((int)GameObjects.ItemAlreadyOwned).SetActive(false);
        Get<GameObject>((int)GameObjects.ItemPurchased).SetActive(false);
    }




}
