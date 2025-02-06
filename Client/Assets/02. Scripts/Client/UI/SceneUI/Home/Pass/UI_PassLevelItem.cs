using System;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using static Define;

public class UI_PassLevelItem : UI_Base
{
    enum GameObjects
    {
        PassLevel,
        ItemBasic,
        ItemPlus,
    }

    bool isInitialized = false;
    int _basicPassListIdx = -1;
    int _plusPassListIdx = -1;

    void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        transform.localScale = Vector3.one;

        isInitialized = true;
    }

    public void SetData(int basicItemIdx, int plusItemIdx, int basicPassListIdx, int plusPassListIdx, int level)
    {
        SetLevelToGetItem(level);
        SetBasicItemImage(basicItemIdx);
        SetPlusItemImage(plusItemIdx);
        _basicPassListIdx = basicPassListIdx;
        _plusPassListIdx = plusItemIdx;
    }

    void SetLevelToGetItem(int level)
    {
        Get<GameObject>((int)GameObjects.PassLevel).transform.GetChild(0).GetComponent<TMP_Text>().text = level.ToString();
    }

    void SetBasicItemImage(int itemIdx)
    {
        var itemData = Managers.Data.ItemDict[itemIdx];

        if (itemData == null) return;

        Get<GameObject>((int)GameObjects.ItemBasic).GetComponent<Image>().sprite = GetItemBGTexture((Define.ItemGrade)itemData.Item_Grade);
        Get<GameObject>((int)GameObjects.ItemBasic).GetComponent<Image>().preserveAspect = true;
        Get<GameObject>((int)GameObjects.ItemBasic).transform.GetChild(0).GetComponent<Image>().sprite = Managers.Resource.LoadSprite(itemData.File_Name);
        Get<GameObject>((int)GameObjects.ItemBasic).transform.GetChild(0).GetComponent<Image>().preserveAspect = true;
    }

    void SetPlusItemImage(int itemIdx)
    {
        var itemData = Managers.Data.ItemDict[itemIdx];

        if (itemData == null) return;

        Get<GameObject>((int)GameObjects.ItemPlus).GetComponent<Image>().sprite = GetItemBGTexture((Define.ItemGrade)itemData.Item_Grade);
        Get<GameObject>((int)GameObjects.ItemPlus).GetComponent<Image>().preserveAspect = true;
        Get<GameObject>((int)GameObjects.ItemPlus).transform.GetChild(0).GetComponent<Image>().sprite = Managers.Resource.LoadSprite(itemData.File_Name);
        Get<GameObject>((int)GameObjects.ItemPlus).transform.GetChild(0).GetComponent<Image>().preserveAspect = true;
    }

    Sprite GetItemBGTexture(Define.ItemGrade Grade)
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

        return sprite;
    }

    public void BindBasicItemEvent(Action<PointerEventData> action)
    {
        Get<GameObject>((int)GameObjects.ItemBasic).BindEvent(action);
    }
}
