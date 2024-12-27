using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class UI_Customize_Item : UI_Base
{
    enum GameObjects
    {
        EquipedImage,
        ItemIcon,
        ItemNameText,
    }

    bool isInitialized = false;
    bool isEquiped = false;

    int ItemIndex = -1;
    int ItemUid = -1;
    UI_Customize parentUI;

    private void Start()
    {
        transform.localScale = Vector3.one;
        if (!isInitialized) Init();

    }

    public void SetParentUI(UI_Customize parentUi)
    {
        parentUI = parentUi;
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

        Get<GameObject>((int)GameObjects.ItemIcon).GetComponent<Image>().sprite = sprite;
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        SetEquip(false);

        isInitialized = true;
    }

    public void SetItem(int uid, int index)
    {
        ItemUid = uid;
        ItemIndex = index;

        var itemData = Managers.Data.GetItemData(index);

        Get<GameObject>((int)GameObjects.ItemNameText).GetComponent<TMP_Text>().text = itemData.Name;

        gameObject.BindEvent((PointerEventData) => {
            parentUI.SetAccessory(ItemIndex);
        }, Define.UIEvent.PointerDown);
    }

    public int GetItemUid()
    {
        return ItemUid;
    }

    public void SetEquip(bool equip)
    {
        isEquiped = equip;
        Get<GameObject>((int)GameObjects.EquipedImage).SetActive(isEquiped);
    }

    public bool IsEquiped()
    {
        return isEquiped;
    }

}
