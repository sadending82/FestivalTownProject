using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_BannerTabs : UI_Base
{
    enum GameObjects
    {
        Banner1Tab,
        Banner2Tab,
    }

    enum Selected
    {
        Banner1Tab,
        Banner2Tab,
    }

    bool isInitialized = false;
    Selected selected = Selected.Banner1Tab;
    UI_RollingBanner parentUI;

    private void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        foreach (GameObjects obj in Enum.GetValues(typeof(GameObjects)))
        {
            Get<GameObject>((int)obj).GetComponent<UI_ButtonTab>().Init();
        }

        isInitialized = true;
    }

    public void SetSelected(int value)
    {
        selected = (Selected)value;
        SelectStuff();
    }

    public void SetParentUI(UI_RollingBanner parentUI)
    {
        this.parentUI = parentUI;
        BindEventAfterSetParentUI();
    }

    public void BindEventAfterSetParentUI()
    {
        var btnUI = Get<GameObject>((int)GameObjects.Banner1Tab).GetComponent<UI_ButtonTab>();
        btnUI.SetEnable(true);

        btnUI.BindEventToButton((PointerEventData) =>
        {
            parentUI.SetBannerOn(0);
        });

        btnUI = Get<GameObject>((int)GameObjects.Banner2Tab).GetComponent<UI_ButtonTab>();
        btnUI.SetEnable(true);

        btnUI.BindEventToButton((PointerEventData) =>
        {
            parentUI.SetBannerOn(1);      
        });

        SelectStuff();
    }

    public void SelectStuff()
    {
        foreach (GameObjects obj in Enum.GetValues(typeof(GameObjects)))
        {
            Get<GameObject>((int)obj).GetComponent<UI_ButtonTab>().SetSelected(false);
        }

        Get<GameObject>((int)selected).GetComponent<UI_ButtonTab>().SetSelected(true);
    }
}
