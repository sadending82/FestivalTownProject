using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class UI_ButtonTab : UI_Base
{
    enum GameObjects
    {
        Button,
        ButtonSelected,
    }

    bool isInitialzed = false;
    bool isEnable = false;
    bool isSelected = false;

    UI_TopPanel parentUI = null;

    private void Start()
    {
        if (!isInitialzed) Init();
    }

    public void SetParentUI(UI_TopPanel ui)
    {
        parentUI = ui;
    }

    public bool IsEnable()
    {
        return isEnable;
    }
    public void SetEnable(bool value)
    {
        isEnable = value;
    }

    public bool IsSelected()
    {
        return isSelected;
    }

    public void SetSelected(bool value)
    {
        if (isEnable)
        {
            isSelected = value;
            SelectedChangeStuff();
        }

    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        isInitialzed = true;
    }

    public void SelectedChangeStuff()
    {
        foreach(GameObjects idx in Enum.GetValues(typeof(GameObjects)))
        {
            Get<GameObject>((int)idx).SetActive(false);
        }

        if (isSelected)
        {
            Get<GameObject>((int)GameObjects.ButtonSelected).SetActive(true);
        }
        else
        {
            Get<GameObject>((int)GameObjects.Button).SetActive(true);
        }
    }

    public void BindEventToButton(Action<PointerEventData> action)
    {
        Get<GameObject>((int)GameObjects.Button).BindEvent(action);
    }
}
