using NetworkProtocol;
using System;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using Unity.VisualScripting;
using UnityEngine;
using static Define;

public class UI_Customize : UI_Scene
{
    enum GameObjects
    {
        SetCustomizingButton,
        UI_CharacterModel,
        CustomizeItemList,
        NickName,
    }

    bool isInitialized = false;
    /// <summary>
    /// ������ �ε���, �κ� ����
    /// </summary>
    Dictionary<int, UI_Customize_Item> ItemSlotDict = new();

    void Start()
    {
        if(!isInitialized) Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        GameObject scrVObj = Get<GameObject>((int)GameObjects.CustomizeItemList).GetComponent<UI_CustomizeItemList>().GetScrollView();
        scrVObj = Get<GameObject>((int)GameObjects.CustomizeItemList).GetComponent<UI_CustomizeItemList>().GetScrollView();
        UI_Customize_ScrollView scrV = scrVObj.GetComponent<UI_Customize_ScrollView>();
        scrV.Init();

        // �κ��丮 ������ ����
        foreach (var item in Managers.Data.InventoryDataList)
        {
            int itemType = Managers.Data.GetItemData(item.Value.ItemCode).Item_Type;
            if (itemType == (int)Define.ItemType.Resource) continue;

            int itemGrade = Managers.Data.GetItemData(item.Value.ItemCode).Item_Grade;

            var ui = Managers.UI.MakeSubItem<UI_Customize_Item>();
            ui.Init();
            ui.SetItem(item.Value.ItemUid, item.Value.ItemCode);
            ui.SetParentUI(this);
            ui.SetItemTexture((Define.ItemGrade)itemGrade);           

            scrV.SetItem(ref ui, (Define.ItemType)itemType);

            ItemSlotDict.TryAdd(item.Value.ItemCode, ui);
        }

        GameObject contentObj = scrV.GetContent();
        if (contentObj != null)
        {
            UI_Customize_ScrViewContent content = contentObj.GetComponent<UI_Customize_ScrViewContent>();
            content.SetHeightByContents();
        }

        //Ŀ���͸���¡ ��ư
        Get<GameObject>((int)GameObjects.SetCustomizingButton).BindEvent((PointerEventData) =>
        {
            sCharacterCustomizing customizing = new sCharacterCustomizing();
            customizing.Init();

            foreach(var itemPair in Managers.Data.ClientLocalCustomizingDataDict)
            {
                Managers.Data.PlayerCustomizingData[itemPair.Key] = itemPair.Value;
            }

            foreach (var itemPair in Managers.Data.PlayerCustomizingData)
            {
                // ������ ������ ������
                if (itemPair.Value.ItemCode == -1) continue;

                // ������ ������ �ش� ������ ����
                EquippedItem eqi = new EquippedItem();

                // itemPair�� Key -> ������ Ÿ��, Value -> ������ ������
                // Define.ItemData �� Type, ItemCode, ItemUid�� �������.
                eqi.itemType = itemPair.Key;
                eqi.itemCode = itemPair.Value.ItemCode;
                eqi.item_UID = ItemSlotDict[itemPair.Value.ItemCode].GetItemUid();

                try
                {
                    customizing.itemList.Add((CustomizingItemType)eqi.itemType, eqi);
                }
                catch
                {
                    Debug.Log("�Ƹ� Ŀ���͸���¡ Ÿ���� ���°� ��������?");
                    continue;
                }
            }

            Managers.Network.GetPacketManager().SendChangeCharacterCustomizingPacket(customizing);
        });

        // �� ������ �ʱ�ȭ �� �����ؿ���
        Managers.Data.ClientLocalCustomizingDataDict.Clear();

        foreach (var itemPair in Managers.Data.PlayerCustomizingData)
        {
            // ���δ� �� ���� ī���ϴ°� ������ ���� ������, ���� ������ ���� ���� �״� �ϴ� �̷���
            Managers.Data.ClientLocalCustomizingDataDict.Add(itemPair.Key, itemPair.Value);

            if(ItemSlotDict.TryGetValue(itemPair.Value.ItemCode, out var itemSlotUi))
            {
                itemSlotUi.SetEquip(true);
            }

        }

        // �� ������ ī�޶� ����
        Camera.main.transform.GetChild(0).gameObject.SetActive(false); // Home�� ī�޶�
        Camera.main.transform.GetChild(1).gameObject.SetActive(true); // Inventory�� ī�޶�

        // ĳ���� �� ������ ���� ĵ���� ����
        this.GetComponent<Canvas>().renderMode = RenderMode.ScreenSpaceCamera;
        this.GetComponent<Canvas>().worldCamera = Camera.main;
        this.GetComponent<Canvas>().planeDistance = Camera.main.nearClipPlane + 0.001f;

        Get<GameObject>((int)GameObjects.NickName).GetComponent<TMP_Text>().text = Managers.Data.GetNickName();

        isInitialized = true;
    }

    public void SetAccessory(int itemIndex)
    {
        // �ε��� ã�ƺôµ� �׵��� ������ �ѱ��
        if (Managers.Data.GetItemData(itemIndex) == null) return;

        // Ÿ�� Ȯ��.
        int itemType = Managers.Data.GetItemData(itemIndex).Item_Type;

        // �ش� Ÿ���� Ŀ�� ������ ��������
        bool result = Managers.Data.ClientLocalCustomizingDataDict.TryGetValue(itemType, out var curItem);

        if (false == result) return;

        // ���� ������ Ŀ���� �ִٸ�
        if (curItem.ItemCode != -1)
        {
            // �ϴ� �ٲ��� �ϴϱ� �������� �����̵� �ϴ� ������ �ְ�
            ItemSlotDict[curItem.ItemCode].SetEquip(false);

            // �� �ʱ�ȭ ���ֱ�.
            Define.ItemData newData = new();
            newData.Type = itemType;
            newData.ItemCode = -1;
            newData.ItemUid = -1;
            Managers.Data.ClientLocalCustomizingDataDict[itemType] = newData;
            Debug.Log($"item Type {itemType}");
        }

        // ���� ��ư ������ ���� �� �ѹ� �� ������ �ƴ϶��
        if (curItem.ItemCode != itemIndex)
        {
            // ���� ����
            ItemSlotDict[itemIndex].SetEquip(true);

            Define.ItemData newData = new();
            newData.Type = itemType;
            newData.ItemCode = itemIndex;
            newData.ItemUid = ItemSlotDict[itemIndex].GetItemUid();
            Managers.Data.ClientLocalCustomizingDataDict[itemType] = newData;
        }

        Get<GameObject>((int)GameObjects.UI_CharacterModel).GetComponent<UI_CharacterModel>().SetInventoryLocalCustomizing();
    }

}
