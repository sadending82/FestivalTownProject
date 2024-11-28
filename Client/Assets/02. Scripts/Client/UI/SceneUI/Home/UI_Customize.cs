using NetworkProtocol;
using System;
using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class UI_Customize : UI_Scene
{
    enum GameObjects
    {
        Panel,
        GridPanel,
        SetCustomizingButton,
        UI_CharacterModel,
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

        // Ȥ�� �𸣴ϱ� �κ��丮 ������ ��� �� �����α�
        GameObject gridPanel = Get<GameObject>((int)GameObjects.GridPanel);
        foreach (Transform child in gridPanel.transform)
        {
            Managers.Resource.Destroy(child.gameObject);
        }

        // �κ��丮 ������ ����
        foreach (Define.ItemData item in Managers.Data.InventoryDataList)
        {
            if (Managers.Data.GetItemData(item.ItemCode).Item_Type == (int)Define.ItemType.Resource) continue;

            var ui = Managers.UI.MakeSubItem<UI_Customize_Item>(gridPanel.transform);
            ui.Init();
            ui.SetItem(item.ItemUid, item.ItemCode);
            ui.SetParentUI(this);

            ItemSlotDict.TryAdd(item.ItemCode, ui);
        }

        //Ŀ���͸���¡ ��ư
        Get<GameObject>((int)GameObjects.SetCustomizingButton).BindEvent((PointerEventData) =>
        {
            sCharacterCustomizing customizing = new sCharacterCustomizing();
            customizing.Init();
            
            foreach(var itemPair in Managers.Data.PlayerCustomizingData)
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
        }

        // �� ������ ī�޶� ����
        Camera.main.transform.GetChild(0).gameObject.SetActive(false); // Home�� ī�޶�
        Camera.main.transform.GetChild(1).gameObject.SetActive(true); // Inventory�� ī�޶�

        // ĳ���� �� ������ ���� ĵ���� ����
        this.GetComponent<Canvas>().renderMode = RenderMode.ScreenSpaceCamera;
        this.GetComponent<Canvas>().worldCamera = Camera.main;
        this.GetComponent<Canvas>().planeDistance = Camera.main.nearClipPlane + 0.001f;

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
