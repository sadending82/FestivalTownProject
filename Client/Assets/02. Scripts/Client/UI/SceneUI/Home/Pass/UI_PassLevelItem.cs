using JetBrains.Annotations;
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
    bool _basicPassRewarded = false;
    bool _plusPassRewarded = false;

    void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        transform.localScale = Vector3.one;

        Get<GameObject>((int)GameObjects.ItemBasic).BindEvent((PointerEventData) =>
        {


            if (_basicPassRewarded == false)
            {
                if (_basicPassListIdx != -1 )
                {
                    int passLevel = Managers.Data.GetPassLevel();
                    if (Managers.Data.PassLevelDataDict[_basicPassListIdx].Level <= passLevel)
                    {
                        Managers.Network.GetPacketManager().SendPassRewardResponsePacket(_basicPassListIdx / 100, (int)PassType.PassBasic, _basicPassListIdx % 100);
                        Debug.Log($"{_basicPassListIdx} Recv Request Send. type : {(int)PassType.PassBasic}");
                    }
                }
            }
        });

        Get<GameObject>((int)GameObjects.ItemPlus).BindEvent((PointerEventData) =>
        {
            if(_plusPassRewarded == false)
            {
                if (_plusPassListIdx != -1)
                {
                    int passLevel = Managers.Data.GetPassLevel();
                    if (Managers.Data.PassLevelDataDict[_plusPassListIdx].Level <= passLevel)
                    {
                        Managers.Network.GetPacketManager().SendPassRewardResponsePacket((_plusPassListIdx / 100) - 1, (int)PassType.PassPlus, _plusPassListIdx % 100);
                        Debug.Log($"{_plusPassListIdx} Recv Request Send. type : {(int)PassType.PassPlus}");
                    }
                }
            }
        });

        SetRewardImage();

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

    public void SetItemRewarded(int idx, bool isRewarded)
    {
        if(_basicPassListIdx == idx)
        {
            _basicPassRewarded = isRewarded;
        }

        if (_plusPassListIdx == idx)
        {
            _plusPassRewarded = isRewarded;
        }

        SetRewardImage();

    }

    void SetRewardImage()
    {
        Get<GameObject>((int)GameObjects.ItemBasic).transform.GetChild(1).gameObject.SetActive(_basicPassRewarded);
        Get<GameObject>((int)GameObjects.ItemPlus).transform.GetChild(1).gameObject.SetActive(_plusPassRewarded);
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

}
