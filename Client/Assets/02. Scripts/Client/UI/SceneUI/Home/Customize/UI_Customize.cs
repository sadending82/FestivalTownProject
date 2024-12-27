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
    /// 아이템 인덱스, 인벤 슬롯
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

        // 인벤토리 아이템 생성
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

        //커스터마이징 버튼
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
                // 데이터 없으면 다음거
                if (itemPair.Value.ItemCode == -1) continue;

                // 데이터 있으면 해당 데이터 설정
                EquippedItem eqi = new EquippedItem();

                // itemPair의 Key -> 아이템 타입, Value -> 아이템 데이터
                // Define.ItemData 는 Type, ItemCode, ItemUid가 들어있음.
                eqi.itemType = itemPair.Key;
                eqi.itemCode = itemPair.Value.ItemCode;
                eqi.item_UID = ItemSlotDict[itemPair.Value.ItemCode].GetItemUid();

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

        // 모델 데이터 초기화 후 복사해오기
        Managers.Data.ClientLocalCustomizingDataDict.Clear();

        foreach (var itemPair in Managers.Data.PlayerCustomizingData)
        {
            // 전부다 한 번에 카피하는게 빠를거 같긴 하지만, 로직 상으론 문제 없을 테니 일단 이렇게
            Managers.Data.ClientLocalCustomizingDataDict.Add(itemPair.Key, itemPair.Value);

            if(ItemSlotDict.TryGetValue(itemPair.Value.ItemCode, out var itemSlotUi))
            {
                itemSlotUi.SetEquip(true);
            }

        }

        // 모델 렌더용 카메라 설정
        Camera.main.transform.GetChild(0).gameObject.SetActive(false); // Home용 카메라
        Camera.main.transform.GetChild(1).gameObject.SetActive(true); // Inventory용 카메라

        // 캐릭터 모델 렌더를 위해 캔버스 설정
        this.GetComponent<Canvas>().renderMode = RenderMode.ScreenSpaceCamera;
        this.GetComponent<Canvas>().worldCamera = Camera.main;
        this.GetComponent<Canvas>().planeDistance = Camera.main.nearClipPlane + 0.001f;

        Get<GameObject>((int)GameObjects.NickName).GetComponent<TMP_Text>().text = Managers.Data.GetNickName();

        isInitialized = true;
    }

    public void SetAccessory(int itemIndex)
    {
        // 인덱스 찾아봤는데 그딴거 없으면 넘기기
        if (Managers.Data.GetItemData(itemIndex) == null) return;

        // 타입 확인.
        int itemType = Managers.Data.GetItemData(itemIndex).Item_Type;

        // 해당 타입의 커마 데이터 가져오기
        bool result = Managers.Data.ClientLocalCustomizingDataDict.TryGetValue(itemType, out var curItem);

        if (false == result) return;

        // 현재 설정된 커마가 있다면
        if (curItem.ItemCode != -1)
        {
            // 일단 바뀌어야 하니까 아이템이 무엇이든 일단 해제해 주고
            ItemSlotDict[curItem.ItemCode].SetEquip(false);

            // 값 초기화 해주기.
            Define.ItemData newData = new();
            newData.Type = itemType;
            newData.ItemCode = -1;
            newData.ItemUid = -1;
            Managers.Data.ClientLocalCustomizingDataDict[itemType] = newData;
            Debug.Log($"item Type {itemType}");
        }

        // 지금 버튼 누른게 같은 거 한번 더 누른게 아니라면
        if (curItem.ItemCode != itemIndex)
        {
            // 새로 설정
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
