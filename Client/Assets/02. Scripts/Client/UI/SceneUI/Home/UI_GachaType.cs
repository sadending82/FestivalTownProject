using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEditor.Rendering;
using UnityEngine;

public class UI_GachaType : UI_Base
{
    enum GameObjects
    {
        Name,
        Image,
        GachaButton,
    }

    bool isIsnitialized = false;
    int GachaTypeNum = 0;

    void Start()
    {
        if (!isIsnitialized)
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

    public void SetRequireResource(int dataIndex)
    {
        if (Managers.Data.GetItemData(dataIndex) == null)
        {
            Debug.Log("그런 재화는 업서요");
            Get<GameObject>((int)GameObjects.GachaButton).GetComponentInChildren<TMP_Text>().text = "1회 뽑기\n\nresourceName";
            return;
        }
        string resourceName = Managers.Data.GetItemData(dataIndex).Name;
        int requireAmount = Managers.Data.GetGachaGroupData(GachaTypeNum).Pay_Item_Value;
        Get<GameObject>((int)GameObjects.GachaButton).GetComponentInChildren<TMP_Text>().text = $"1회 뽑기\n\n{requireAmount} {resourceName}";
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.GachaButton).BindEvent((PointerEventData) =>
        {
            var popup = Managers.UI.ShowPopUpUI<UI_GachaPopup>();
            popup.SetGachaType(GachaTypeNum);
        });

        GetComponent<RectTransform>().localPosition = Vector3.zero;
        GetComponent<RectTransform>().localScale = Vector3.one;

        isIsnitialized = true;
    }
}
