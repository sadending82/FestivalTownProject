using ExcelDataStructure;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using static Autodesk.Fbx.FbxAnimCurveDef;

public class UI_GachaType : UI_Base
{
    enum GameObjects
    {
        Name,
        Image,
        GachaButton,
    }

    bool isInitialized = false;
    int GachaTypeNum = 0;

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
    }

    public void SetName(string name)
    {
        if (Get<GameObject>((int)GameObjects.Name) == null) { return; }
        Get<GameObject>((int)GameObjects.Name).GetComponent<TMP_Text>().text = name;

    }

    public void SetRequireResource(int dataIndex1, int dataIndex2)
    {
        if (Managers.Data.GetItemData(dataIndex1) == null)
        {
            Debug.Log("�׷� ��ȭ�� ������");
            Get<GameObject>((int)GameObjects.GachaButton).GetComponentInChildren<TMP_Text>().text = "1ȸ �̱�\n\nresourceName";
            return;
        }

        if (Managers.Data.GetItemData(dataIndex2) == null)
        {
            RequireResource1Index = dataIndex1;
            string resourceName1 = Managers.Data.GetItemData(dataIndex1).Name;
            int requireAmount1 = Managers.Data.GetGachaGroupData(GachaTypeNum).Pay_Item1_Value;
            RequireResource1Amount = requireAmount1;

            Get<GameObject>((int)GameObjects.GachaButton).GetComponentInChildren<TMP_Text>().text = $"1ȸ �̱�\n" +
    $"{requireAmount1} {resourceName1}";
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
            Get<GameObject>((int)GameObjects.GachaButton).GetComponentInChildren<TMP_Text>().text = $"1ȸ �̱�\n{requireAmount2} {resourceName2} or\n" +
                $"{requireAmount1} {resourceName1}";
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
                    popup.SetText($"{RequireResource2Amount} {ie.Name}�� ����� �̱⸦ �����Ͻðڽ��ϱ�?");
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
                popup.SetText($"{RequireResource1Amount} {ie.Name}�� ����� �̱⸦ �����Ͻðڽ��ϱ�?");
                return;
            }

            var ui = Managers.UI.ShowPopUpUI<UI_Notice>();
            ui.Init();
            ui.NoticeTextChange("�̱⿡ �ʿ��� ��ᰡ �����մϴ�.");
            ui.BindPopupCloseEvent();

        });

        GetComponent<RectTransform>().localPosition = Vector3.zero;
        GetComponent<RectTransform>().localScale = Vector3.one;

        isInitialized = true;
    }
}
