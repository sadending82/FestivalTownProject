using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UIElements;

public class UI_SettingTab : UI_Base
{
    enum GameObjects
    {
        Sound,
        Graphic,
        Control,
    }

    enum Selected
    {
        Sound,
        Graphic,
        Control,
    }

    bool isInitialized = false;
    Selected selected = Selected.Sound;
    UI_SettingPanel parentUI;

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

    public void SetParentUI(UI_SettingPanel parentUI)
    {
        this.parentUI = parentUI;
        BindEventAfterSetParentUI();
    }

    public void BindEventAfterSetParentUI()
    {
        var btnUI = Get<GameObject>((int)GameObjects.Sound).GetComponent<UI_ButtonTab>();
        btnUI.SetEnable(true);

        btnUI.BindEventToButton((PointerEventData) =>
        {
            parentUI.SelectDetailUI(0);
        });

        btnUI = Get<GameObject>((int)GameObjects.Graphic).GetComponent<UI_ButtonTab>();
        btnUI.SetEnable(true);

        btnUI.BindEventToButton((PointerEventData) =>
        {
            parentUI.SelectDetailUI(1);
        });

        btnUI = Get<GameObject>((int)GameObjects.Control).GetComponent<UI_ButtonTab>();
        btnUI.SetEnable(enabled);

        btnUI.BindEventToButton((PointerEventData) =>
        {

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
