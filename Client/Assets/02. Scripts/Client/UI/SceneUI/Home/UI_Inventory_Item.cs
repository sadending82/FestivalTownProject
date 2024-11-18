using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UI_Inventory_Item : UI_Base
{
    enum GameObjects
    {
        SelectedImage,
        EquipedImage,
        ItemIcon,
        ItemNameText,
    }

    bool isInitialized = false;
    bool isSelected = false;
    bool isEquiped = false;

    int ItemIndex = -1;
    int ItemUid = -1;
    UI_Inventory parentUI;

    private void Start()
    {
        transform.localScale = Vector3.one;
        if (!isInitialized) Init();

    }

    public void SetParentUI(UI_Inventory parentUi)
    {
        parentUI = parentUi;
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        SetEquip(false);
        SetSelect(false);

        isInitialized = true;
    }

    public void SetItem(int uid, int index)
    {
        ItemUid = uid;
        ItemIndex = index;

        var itemData = Managers.Data.GetItemData(index);

        Get<GameObject>((int)GameObjects.ItemNameText).GetComponent<TMP_Text>().text = itemData.Name;

        Get<GameObject>((int)GameObjects.ItemIcon).BindEvent((PointerEventData) => {
            parentUI.SetAccessory(ItemIndex);
        });
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

    public void SetSelect(bool select)
    {
        isSelected = select;
        Get<GameObject>((int)GameObjects.SelectedImage).SetActive(isSelected);
    }

    public bool IsEquiped()
    {
        return isEquiped;
    }

    public bool IsSelected()
    {
        return isSelected; 
    }
}
