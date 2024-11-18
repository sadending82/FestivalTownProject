using NetworkProtocol;
using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class UI_Inventory : UI_PopUp
{
    enum GameObjects
    {
        Panel,
        GridPanel,
        ExitButton,
        SetCustomizingButton,
    }

    bool isInitialized = false;
    /// <summary>
    /// 아이템 인덱스, 인벤 슬롯
    /// </summary>
    Dictionary<int, UI_Inventory_Item> ItemSlotDict = new();
    /// <summary>
    /// 아이템 타입, 아이템 인덱스
    /// </summary>
    Dictionary<int, int> CurrentItem = new();

    void Start()
    {
        if(!isInitialized) Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        CurrentItem.Add((int)Define.ItemType.Skin, -1);
        CurrentItem.Add((int)Define.ItemType.Accessory_Head, -1);
        CurrentItem.Add((int)Define.ItemType.Accessory_Face, -1);
        CurrentItem.Add((int)Define.ItemType.Accessory_Back, -1);
        CurrentItem.Add((int)Define.ItemType.Emo, -1);

        GameObject gridPanel = Get<GameObject>((int)GameObjects.GridPanel);
        foreach (Transform child in gridPanel.transform)
        {
            Managers.Resource.Destroy(child.gameObject);
        }

        foreach (Define.ItemData item in Managers.Data.InventoryDataList)
        {
            if (Managers.Data.GetItemData(item.ItemCode).Item_Type == (int)Define.ItemType.Resource) continue;

            var ui = Managers.UI.MakeSubItem<UI_Inventory_Item>(gridPanel.transform);
            ui.Init();
            ui.SetItem(item.ItemUid, item.ItemCode);
            ui.SetParentUI(this);

            ItemSlotDict.TryAdd(item.ItemCode, ui);
        }

        Get<GameObject>((int)GameObjects.ExitButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.ClosePopUpUI();
        });

        Get<GameObject>((int)GameObjects.SetCustomizingButton).BindEvent((PointerEventData) =>
        {
            sCharacterCustomizing customizing = new sCharacterCustomizing();
            customizing.Init();
            
            foreach(var itemPair in CurrentItem)
            {
                if (itemPair.Value == -1) continue;
                EquippedItem eqi = new EquippedItem();
                eqi.itemType = itemPair.Key;
                eqi.itemCode = itemPair.Value;
                
                eqi.item_UID = ItemSlotDict[itemPair.Value].GetItemUid();
                try
                {
                    customizing.itemList.Add((CustomizingItemType)eqi.itemType, eqi);
                    
                }
                catch
                {
                    Debug.Log("아마 커스터마이징 타입이 없는거 같은데요?");
                    continue;
                }
            }

            Managers.Network.GetPacketManager().SendChangeCharacterCustomizingPacket(customizing);
        });

        isInitialized = true;
    }

    public void SetAccessory(int itemIndex)
    {
        if (Managers.Data.GetItemData(itemIndex) == null) return;

        int itemType = Managers.Data.GetItemData(itemIndex).Item_Type;

        CurrentItem.TryGetValue(itemType, out int curItem);

        if (curItem != -1)
        {
            ItemSlotDict[curItem].SetEquip(false);
            CurrentItem[itemType] = -1;
        }

        if (curItem != itemType)
        {
            ItemSlotDict[itemIndex].SetEquip(true);
            CurrentItem[itemType] = itemIndex;
        }
    }

}
