using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_Inventory : UI_PopUp
{
    enum GameObjects
    {
        Panel,
        GridPanel,
        ExitButton,
    }

    bool isInitialized = false;

    void Start()
    {
        if(!isInitialized) Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));

        GameObject gridPanel = Get<GameObject>((int)GameObjects.GridPanel);
        foreach (Transform child in gridPanel.transform)
        {
            Managers.Resource.Destroy(child.gameObject);
        }

        foreach (Define.ItemData item in Managers.Data.InventoryDataList)
        {
            var ui = Managers.UI.MakeSubItem<UI_Inventory_Item>(gridPanel.transform);
            ui.Init();
            ui.SetItem(item.ItemCode);
            ui.SetParentUI(this);
        }

        Get<GameObject>((int)GameObjects.ExitButton).BindEvent((PointerEventData) =>
        {
            Managers.UI.ClosePopUpUI();
        });

        isInitialized = true;
    }
}
