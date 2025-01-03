using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class UI_ItemInfo : UI_Base
{
    enum GameObjects
    {
        BG,
        Image,
        Name,
    }

    bool isInitialized = false;

    int ItemIndex = -1;

    private void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        transform.localScale = Vector3.one;
        transform.localPosition = Vector3.zero;

        isInitialized = true;
    }

    public void SetItemTexture(Define.ItemGrade Grade)
    {
        Sprite sprite = null;
        switch (Grade)
        {
            case Define.ItemGrade.Normal:
                sprite = Managers.Resource.LoadSprite("NormalBG");
                break;
            case Define.ItemGrade.Rare:
                sprite = Managers.Resource.LoadSprite("RareBG");
                break;
            case Define.ItemGrade.Unique:
                sprite = Managers.Resource.LoadSprite("UniqueBG");
                break;
        }

        Get<GameObject>((int)GameObjects.BG).GetComponent<Image>().sprite = sprite;
    }

    public void SetItem(int index)
    {
        ItemIndex = index;

        var itemData = Managers.Data.GetItemData(index);
        Sprite sp = Managers.Resource.LoadSprite(itemData.File_Name);
        if (sp == null) return;
        Get<GameObject>((int)GameObjects.Image).GetComponent<Image>().sprite = Managers.Resource.LoadSprite(itemData.File_Name);
        Get<GameObject>((int)GameObjects.Image).GetComponent<Image>().preserveAspect = true;

    }

    public void SetName(string name)
    {
        Get<GameObject>((int)GameObjects.Name).GetComponent<TMP_Text>().text = name;
    }
}
