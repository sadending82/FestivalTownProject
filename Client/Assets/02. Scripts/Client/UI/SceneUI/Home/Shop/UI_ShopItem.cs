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
            Get<GameObject>((int)GameObjects.ItemCurrency).transform.GetChild(0).GetComponent<Image>(); // ���� �̹��� ������ ��� �ϸ� �����ϱ�.
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
    /// �������� ����������, �������� �̹� �����ߴ��� Ȯ���ϰ�, �ش��ϴ� ���·� �����ϴ� �Լ�
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
