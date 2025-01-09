using ExcelDataStructure;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using TMPro;
using Unity.VisualScripting;
using UnityEngine;
using static UnityEngine.Rendering.DebugUI;

public class UI_GachaType : UI_Base
{
    enum GameObjects
    {
        Name,
        Image,
        GachaButton,
        ItemInfoPanel,
    }

    bool isInitialized = false;
    int GachaTypeNum = 0;
    int GachaGroupIndex = 0;

    int RequireResource1Index = 0;
    int RequireResource1Amount = 0;
    int RequireResource2Index = 0;
    int RequireResource2Amount = 0;

    void Start()
    {
        if (!isInitialized)
        {
            Init();
        }

    }

    public void SetGachaType(int value)
    {
        GachaTypeNum = value;
        SetGachaGroupIndex(Managers.Data.GachaGroupDict[value].Gacha_Group);
    }

    public void SetGachaGroupIndex(int value)
    {
        GachaGroupIndex = value; 
    }

    public void SetRequireResource(int dataIndex1, int dataIndex2)
    {
        if (Managers.Data.GetItemData(dataIndex1) == null)
        {
            Debug.Log("그런 재화는 업서요");
            return;
        }

        if (Managers.Data.GetItemData(dataIndex2) == null)
        {
            RequireResource1Index = dataIndex1;
            string resourceName1 = Managers.Data.GetItemData(dataIndex1).Name;
            int requireAmount1 = Managers.Data.GetGachaGroupData(GachaTypeNum).Pay_Item1_Value;
            RequireResource1Amount = requireAmount1;
        }
        else
        {
            RequireResource1Index = dataIndex1;
            RequireResource2Index = dataIndex2;

            string resourceName1 = Managers.Data.GetItemData(dataIndex1).Name;
            int requireAmount1 = Managers.Data.GetGachaGroupData(GachaTypeNum).Pay_Item1_Value;
            RequireResource1Amount = requireAmount1;
            string resourceName2 = Managers.Data.GetItemData(dataIndex2).Name;
            int requireAmount2 = Managers.Data.GetGachaGroupData(GachaTypeNum).Pay_Item2_Value;
            RequireResource2Amount = requireAmount2;
        }
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.GachaButton).BindEvent((PointerEventData) =>
        {
            if(Managers.Data.GetItemData(RequireResource2Index) != null)
            { 
                var r2 = Managers.Data.GetPlayerData(RequireResource2Index);
            
                if (r2 >= RequireResource2Amount)
                {
                    var popup = Managers.UI.ShowPopUpUI<UI_GachaPopup>();
                    popup.Init();
                    popup.SetGachaType(GachaTypeNum);
                    ItemEntity ie = Managers.Data.GetItemData(RequireResource2Index);
                    popup.SetText($"{RequireResource2Amount} {ie.Name}을 사용해 뽑기를 진행하시겠습니까?");
                    return;
                }
            }

            var r1 = Managers.Data.GetPlayerData(RequireResource1Index);
            if (r1 >= RequireResource1Amount)
            {
                var popup = Managers.UI.ShowPopUpUI<UI_GachaPopup>();
                popup.Init();
                popup.SetGachaType(GachaTypeNum);
                ItemEntity ie = Managers.Data.GetItemData(RequireResource1Index);
                popup.SetText($"{RequireResource1Amount} {ie.Name}을 사용해 뽑기를 진행하시겠습니까?");
                return;
            }

            var ui = Managers.UI.ShowPopUpUI<UI_Notice>();
            ui.Init();
            ui.NoticeTextChange("뽑기에 필요한 재료가 부족합니다.");
            ui.BindPopupCloseEvent();

        });

        GetComponent<RectTransform>().localPosition = Vector3.zero;
        GetComponent<RectTransform>().localScale = Vector3.one;

        var itemInfoPanel = Get<GameObject>((int)GameObjects.ItemInfoPanel).GetComponent<UI_ItemInfoPanel>();
        
        if (itemInfoPanel == null) return;

        itemInfoPanel.Init();
        List<KeyValuePair<int, GachaGroupItemEntity>> tempData = new();


        foreach (var data in Managers.Data.GachaGroupItemDict)
        {
            if (data.Value.Gacha_Group != GachaGroupIndex) continue;

            if (data.Value.Is_Promote == false) continue;

            tempData.Add(data);

        }

        tempData.Sort((KeyValuePair<int, GachaGroupItemEntity> a, KeyValuePair<int, GachaGroupItemEntity> b) => {
            return (Managers.Data.ItemDict[a.Value.Reward_Item_Index].Index).CompareTo(Managers.Data.ItemDict[b.Value.Reward_Item_Index].Index);
            });

        var newData = tempData.OrderByDescending(n1 => Managers.Data.ItemDict[n1.Value.Reward_Item_Index].Item_Grade);

        int cnt = 0;

        foreach (var data in newData)
        {
            itemInfoPanel.AddItemInfoList(data.Value.Reward_Item_Index);
            cnt++;

            if (cnt >= 12) break;
        }

        SetGachaphoneCustomizing();
        isInitialized = true;
    }

    private void SetGachaphoneCustomizing()
    {
        switch (GachaTypeNum)
        {
            // 냥냥 가챠
            case 100011:
                {
                    Get<GameObject>((int)GameObjects.Name).transform.GetChild(0).gameObject.SetActive(true);
                    Get<GameObject>((int)GameObjects.Image).transform.GetChild(0).gameObject.SetActive(true);
                    Get<GameObject>((int)GameObjects.GachaButton).transform.GetChild(0).gameObject.SetActive(true);
                }
                break;
            // 울트라 냥냥 가챠
            case 100012:
                {
                    Get<GameObject>((int)GameObjects.Name).transform.GetChild(1).gameObject.SetActive(true);
                    Get<GameObject>((int)GameObjects.Image).transform.GetChild(1).gameObject.SetActive(true);
                    Get<GameObject>((int)GameObjects.GachaButton).transform.GetChild(1).gameObject.SetActive(true);
                }
                break;
            case 100013:
                {

                }
                break;
            case 100014:
                {
                    Get<GameObject>((int)GameObjects.Name).transform.GetChild(2).gameObject.SetActive(true);
                    Get<GameObject>((int)GameObjects.Image).transform.GetChild(2).gameObject.SetActive(true);
                    Get<GameObject>((int)GameObjects.GachaButton).transform.GetChild(0).gameObject.SetActive(true);
                }
                break;
            default:
                {
                    Debug.Log("ERROR !!! SetGachaType(): Wrong GachaType !!!");
                }
                break;
        }
    }
}
