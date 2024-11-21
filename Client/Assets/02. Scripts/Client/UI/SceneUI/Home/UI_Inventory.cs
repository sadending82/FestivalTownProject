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

    void Start()
    {
        if(!isInitialized) Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        Camera.main.transform.GetChild(0).gameObject.SetActive(false); // Home용 카메라
        Camera.main.transform.GetChild(1).gameObject.SetActive(true); // Inventory용 카메라

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
            var sceneUi = Managers.UI.GetCurrentSceneUI();
            if (sceneUi != null && sceneUi.GetComponent<UI_HomeStart>() != null)
            {
                sceneUi.GetComponent<UI_HomeStart>().SetCustomizing();
                Camera.main.transform.GetChild(0).gameObject.SetActive(true); // Home용 카메라
                Camera.main.transform.GetChild(1).gameObject.SetActive(false); // Inventory용 카메라
            }
            Managers.UI.ClosePopUpUI();
            
        });

        Get<GameObject>((int)GameObjects.SetCustomizingButton).BindEvent((PointerEventData) =>
        {
            sCharacterCustomizing customizing = new sCharacterCustomizing();
            customizing.Init();
            
            foreach(var itemPair in Managers.Data.PlayerCustomizingData)
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

        this.GetComponent<Canvas>().renderMode = RenderMode.ScreenSpaceCamera;
        this.GetComponent<Canvas>().worldCamera = Camera.main;
        this.GetComponent<Canvas>().planeDistance = Camera.main.nearClipPlane + 0.001f;

        isInitialized = true;
    }

    public void SetAccessory(int itemIndex)
    {
        // 그딴거 없으면 넘기기
        if (Managers.Data.GetItemData(itemIndex) == null) return;

        // 타입 확인.
        int itemType = Managers.Data.GetItemData(itemIndex).Item_Type;

        // 해당 타입의 커마 데이터 가져오기
        Managers.Data.PlayerCustomizingData.TryGetValue(itemType, out var curItem);

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
            Managers.Data.PlayerCustomizingData[itemType] = newData;
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
            Managers.Data.PlayerCustomizingData[itemType] = newData;
        }
    }

}
