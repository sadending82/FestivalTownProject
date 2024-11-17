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
    UI_Inventory parentUI;

    private void Start()
    {
        if (!isInitialized) Init();
    }

    public void SetParentUI(UI_Inventory parentUi)
    {
        parentUI = parentUi;
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        isInitialized = true;
    }

    public void SetItem(int index)
    {
        var itemData = Managers.Data.GetItemData(index);

        Get<GameObject>((int)GameObjects.ItemNameText).GetComponent<TMP_Text>().text = itemData.Name;
    }

    public void SetEquip(bool equip)
    {
        isEquiped = equip;
    }

    public void SetSelect(bool select)
    {
        isSelected = select;
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
